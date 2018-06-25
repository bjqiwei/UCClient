// ATLWebSocket.cpp : Implementation of CATLWebSocket
#include "stdafx.h"
#include "ATLWebSocket.h"
#include "codingHelper.h"
#include <thread>
#include <json/json.h>

static std::atomic_ulong g_WebSocketServerReferce = 0;
static std::thread g_WebSocketServerTh;
static WebSocket::WebSocketServer * wsserver = nullptr;

static HRESULT JSCreateObject(IDispatch * pScript, CComBSTR objName, DISPPARAMS params, IDispatch ** newObject)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSCreateObject");
	VARIANT  rVar;
	VariantInit(&rVar);
	DISPID idMethod = 0;
	CComQIPtr<IDispatchEx> pScriptEx;

	HRESULT hr = pScript->QueryInterface(IID_IDispatchEx, (void**)&pScriptEx);
	if (!SUCCEEDED(hr)) {
		LOG4CPLUS_ERROR(log, __FUNCTION__ " cannot find interface IDispatchEx, :" << _bstr_t(objName) << ",ErrorCode:" << hr);
		return hr;
	}

	hr = pScriptEx->GetDispID(objName, 0, &idMethod);

	if (!SUCCEEDED(hr)) {
		LOG4CPLUS_ERROR(log, __FUNCTION__ " cannot find " << _bstr_t(objName) << ",ErrorCode:" << hr);
		return hr;
	}

	hr = pScriptEx->InvokeEx(idMethod, LOCALE_SYSTEM_DEFAULT, DISPATCH_CONSTRUCT,
		&params, &rVar, NULL, NULL);

	if (SUCCEEDED(hr) && rVar.vt == VT_DISPATCH) {
		*newObject = rVar.pdispVal;
	}
	else
		LOG4CPLUS_ERROR(log, __FUNCTION__ "," << _bstr_t(objName) << ", ErrorCode:" << hr);

	return hr;
}

static HRESULT JSNewObject(IDispatch * pScript, IDispatch ** newObject)
{
	DISPPARAMS dpNoArgs = { NULL, NULL, 0, 0 };
	return JSCreateObject(pScript, "Object", dpNoArgs, newObject);
}

static HRESULT JSNewArray(IDispatch * pScript, IDispatch ** newArrary)
{
	DISPPARAMS dpNoArgs = { NULL, NULL, 0, 0 };
	return JSCreateObject(pScript, "Array", dpNoArgs, newArrary);
}

static HRESULT JSNewNumber(IDispatch * pScript, IDispatch ** newNumber, CComVariant value)
{
	DISPPARAMS params = { &value, NULL, 1, 0 };
	return JSCreateObject(pScript, "Number", params, newNumber);
}

static HRESULT JSGetProperty(IDispatch* pDisp, CComBSTR pName, VARIANT * value)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSGetProperty");
	DISPID dispId;
	HRESULT hr = S_OK;
	DISPPARAMS noArgs = { NULL, NULL, 0, 0 };

	hr = pDisp->GetIDsOfNames(IID_NULL, &pName, 1, LOCALE_USER_DEFAULT, &dispId);

	if (FAILED(hr)) {
		LOG4CPLUS_ERROR(log, __FUNCTION__ " no " << _bstr_t(pName) << " property," << hr);
		return hr;
	}

	hr = pDisp->Invoke(dispId, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &noArgs, value, NULL, NULL);

	return hr;
}

static HRESULT JSSetProperty(IDispatch* pDisp, CComBSTR pName, CComVariant value)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSSetProperty");
	DISPID dispId;
	HRESULT hr = S_OK;
	DISPID putid = DISPID_PROPERTYPUT;
	DISPPARAMS params = { &value, &putid, 1, 1 };
	CComQIPtr<IDispatchEx> pDispEx;

	//IE8 不支持ULONGLONG 类型
	if (value.vt == VT_UI8) {
		value.vt = VT_UI4;
		value.ulVal = value.ullVal;
	}

	hr = pDisp->QueryInterface(IID_IDispatchEx, (void**)&pDispEx);
	if (FAILED(hr)) {
		LOG4CPLUS_ERROR(log, __FUNCTION__ << "," << _bstr_t(pName) << " QueryInterface IDispatchEx error, " << hr);
		return hr;
	}

	hr = pDispEx->GetDispID(pName, fdexNameEnsure, &dispId);

	if (FAILED(hr)) {
		LOG4CPLUS_ERROR(log, __FUNCTION__ ", " << _bstr_t(pName) << "," << hr);
		return hr;
	}

	hr = pDispEx->InvokeEx(dispId, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUTREF, &params, NULL, NULL, NULL);
	if (FAILED(hr))
	{
		LOG4CPLUS_ERROR(log, __FUNCTION__ << ", " << _bstr_t(pName) << "," << hr);
	}

	return hr;
}

static HRESULT JSSetValue(IDispatch* pDisp, CComVariant value)
{
	return JSSetProperty(pDisp, "value", value);
}

static HRESULT JSAddArrayElement(IDispatch* pDisp, CComVariant value)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSAddArrayElement");
	HRESULT hr = S_OK;
	DISPID      dispid = 0;
	CComBSTR funcName(L"push");
	DISPPARAMS params = { &value, NULL, 1, 0 };

	hr = pDisp->GetIDsOfNames(IID_NULL, &funcName, 1, LOCALE_USER_DEFAULT, &dispid);

	if (FAILED(hr)) {
		LOG4CPLUS_ERROR(log, __FUNCTION__ "," << hr);
		return hr;
	}

	hr = pDisp->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
	if (FAILED(hr)) {
		LOG4CPLUS_ERROR(log, __FUNCTION__ << "," << hr);
	}

	return hr;

}

static HRESULT JSGetArrayElementOfIndex(IDispatch* pDisp, int index, VARIANT * pValue)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSGetArrayElementOfIndex");

	CComVariant varName(index, VT_I4);   // 数组下标
	DISPPARAMS noArgs = { NULL, NULL, 0, 0 };
	DISPID dispId;
	HRESULT hr = S_OK;

	varName.ChangeType(VT_BSTR);         // 将数组下标转为数字型，以进行GetIDsOfNames
	hr = pDisp->GetIDsOfNames(IID_NULL, &varName.bstrVal, 1, LOCALE_USER_DEFAULT, &dispId);
	if (FAILED(hr)) {
		LOG4CPLUS_ERROR(log, ", Get Array Index:" << _bstr_t(varName) << ",ErrorCode:" << hr);
		return hr;
	}

	hr = pDisp->Invoke(dispId, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &noArgs, pValue, NULL, NULL);
	if (FAILED(hr)) {
		LOG4CPLUS_ERROR(log, ",Get Array Index:" << _bstr_t(varName) << ",ErrorCode:" << hr);
	}

	return hr;

}

class ATLWSclient :public WebSocket::WebSocketClient
{
public:
	explicit ATLWSclient(struct lws_context *context, CATLWebSocket * module)
		:WebSocketClient(context),m_module(module)
	{
		this->log = log4cplus::Logger::getInstance("ATLWSclient");
		LOG4CPLUS_TRACE(log, m_SessionId << "construction");
	};

	~ATLWSclient()
	{
		LOG4CPLUS_TRACE(log, m_SessionId << "deconstruct");
	}

	virtual void OnOpen() override
	{
		LOG4CPLUS_TRACE(log, m_SessionId << __FUNCTION__ ", start");
		PostMessage(m_module->m_hWnd, CATLWebSocket::WM_onOpen, NULL, NULL);
		LOG4CPLUS_TRACE(log, m_SessionId << __FUNCTION__ ", end");
	}

	virtual void OnSend() override
	{
		//LOG4CPLUS_DEBUG(log, m_SessionId << "OnSend");
	}

	virtual void OnClose(const std::string & errorCode) override
	{
		LOG4CPLUS_TRACE(log, m_SessionId << __FUNCTION__ ", start");
		std::string * param = new std::string(errorCode);
		PostMessage(m_module->m_hWnd, CATLWebSocket::WM_onClose, NULL, reinterpret_cast<LPARAM>(param));
		LOG4CPLUS_TRACE(log, m_SessionId << __FUNCTION__ ", end");
	};

	virtual void OnError(const std::string & errorCode) override
	{
		LOG4CPLUS_TRACE(log, m_SessionId << __FUNCTION__ ", start");
		std::string * param = new std::string(errorCode);
		PostMessage(m_module->m_hWnd, CATLWebSocket::WM_onError, NULL, reinterpret_cast<LPARAM>(param));
		LOG4CPLUS_TRACE(log, m_SessionId << __FUNCTION__ ", end");
	};

	virtual void OnMessage(const std::string & message) override
	{
		//LOG4CPLUS_TRACE(log, m_SessionId << __FUNCTION__ ", start");
		std::string * param = new std::string(message);
		PostMessage(m_module->m_hWnd, CATLWebSocket::WM_onMessage, NULL, reinterpret_cast<LPARAM>(param));
		//LOG4CPLUS_TRACE(log, m_SessionId << __FUNCTION__ ", end");
	};

private:
	CATLWebSocket * m_module;
	friend class CATLWebSocket;

};

static void WebSocketThread()
{
	log4cplus::Logger log = log4cplus::Logger::getInstance("WebSocketThread");
	LOG4CPLUS_INFO(log, "Start...");
	wsserver = new WebSocket::WebSocketServer();
	wsserver->InitInstance();

	while (g_WebSocketServerReferce.fetch_add(0) > 0) {
		wsserver->Loop(200);
	}

	wsserver->UnInitInstance();
	delete wsserver;
	LOG4CPLUS_INFO(log, "Stoped.");
	log4cplus::threadCleanup();
}

void InitWebSocket()
{
	if (g_WebSocketServerReferce.fetch_add(1) != 0)
		return;
	g_WebSocketServerTh = std::thread(WebSocketThread);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void UnInitWebSocket()
{
	if (g_WebSocketServerReferce.fetch_sub(1) == 1) {
		g_WebSocketServerTh.join();
	}
}

CATLWebSocket::CATLWebSocket()
{
	m_bWindowOnly = TRUE;
	CalcExtent(m_sizeExtent);
	this->log = log4cplus::Logger::getInstance("ATLWebSocket");
	LOG4CPLUS_INFO(log, "SDK Version:" << getVersion());
	LOG4CPLUS_INFO(log, "SDK:" << utf8ModuleFilename);
	InitWebSocket();
	while (wsserver->GetContext() == nullptr){
		static int i = 0;
		i++;
		std::this_thread::sleep_for(std::chrono::seconds(1));

		if (i>5)
			break;
	}
	m_wsclient = new ATLWSclient(wsserver->GetContext(), this);

}

CATLWebSocket::~CATLWebSocket()
{
}

// CATLWebSocket

LRESULT CATLWebSocket::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// TODO: Add your message handler code here and/or call default
	LOG4CPLUS_TRACE(log, m_wsclient->m_SessionId << __FUNCTION__ << " m_hWnd:" << m_hWnd);
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	if (m_UserAgent.empty())
	{
		get_userAgent(m_UserAgent);
	}

	LOG4CPLUS_DEBUG(log, m_wsclient->m_SessionId << "UserAgent:" << m_UserAgent);
	bHandled = FALSE;
	return TRUE;
}


LRESULT CATLWebSocket::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// TODO: Add your message handler code here and/or call default
	LOG4CPLUS_DEBUG(log, m_wsclient->m_SessionId << __FUNCTION__ ":" << m_hWnd);
	delete m_wsclient;
	m_wsclient = nullptr;
	UnInitWebSocket();
	bHandled = FALSE;
	return 0;
}

LRESULT CATLWebSocket::onOpen(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LOG4CPLUS_DEBUG(log, m_wsclient->m_SessionId << "Fire_OnOpen");
	if (m_onopen){
		IDispatch * evt = nullptr;
		if (m_Script == NULL) {
			get_Script(&m_Script);
		}
		JSNewObject(m_Script, &evt);
		CComVariant avarParams[1];
		avarParams[0] = evt;
		avarParams[0].vt = VT_DISPATCH;
		DISPPARAMS params = { avarParams, NULL, 1, 0 };
		LRESULT hr = m_onopen->Invoke((DISPID)DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
	}
	return S_OK;
}



LRESULT CATLWebSocket::onMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	std::string * param = reinterpret_cast<std::string *>(lParam);
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(*param,root) 
		&& root["type"].isString() && root["type"].asString() == "ping"){
	}
	else
		LOG4CPLUS_DEBUG(log, m_wsclient->m_SessionId << "Fire_OnMessage:" << *param);

	if (m_onmessage) {
		IDispatch * evt = nullptr;
		if (m_Script == NULL) {
			get_Script(&m_Script);
		}
		JSNewObject(m_Script, &evt);
		std::wstring uParam = Utf82Unicode(*param);
		JSSetProperty(evt, "data", uParam.c_str());

		CComVariant avarParams[1];
		avarParams[0] = evt;
		avarParams[0].vt = VT_DISPATCH;
		DISPPARAMS params = { avarParams, NULL, 1, 0 };
		LRESULT hr = m_onmessage->Invoke((DISPID)DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
	}
	delete param;
	return S_OK;
}

LRESULT CATLWebSocket::onClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	std::string * param = reinterpret_cast<std::string *>(lParam);
	LOG4CPLUS_DEBUG(log, m_wsclient->m_SessionId << "Fire_OnClose:" << *param);
	if (m_onclose) {
		IDispatch * evt = nullptr;
		if (m_Script == NULL) {
			get_Script(&m_Script);
		}
		JSNewObject(m_Script, &evt);

		CComVariant avarParams[1];
		avarParams[0] = evt;
		avarParams[0].vt = VT_DISPATCH;
		DISPPARAMS params = { avarParams, NULL, 1, 0 };
		LRESULT hr = m_onclose->Invoke((DISPID)DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
	}
	delete param;
	return S_OK;
}

LRESULT CATLWebSocket::onError(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	std::string * param = reinterpret_cast<std::string *>(lParam);
	LOG4CPLUS_DEBUG(log, m_wsclient->m_SessionId << "Fire_OnError:" << *param);
	if (m_onerror) {
		IDispatch * evt = nullptr;
		if (m_Script == NULL) {
			get_Script(&m_Script);
		}
		JSNewObject(m_Script, &evt);

		CComVariant avarParams[1];
		avarParams[0] = evt;
		avarParams[0].vt = VT_DISPATCH;
		DISPPARAMS params = { avarParams, NULL, 1, 0 };
		LRESULT hr = m_onerror->Invoke((DISPID)DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
	}
	delete param;
	return S_OK;
}

STDMETHODIMP CATLWebSocket::setwsurl(BSTR url)
{
	// TODO: Add your implementation code here
	std::string strurl = Unicode2Utf8(url ? url : L"");
	m_wsclient->SetWSUrl(strurl);
	m_wsclient->Open();
	return S_OK;
}


STDMETHODIMP CATLWebSocket::get_m_readyState(LONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_wsclient->GetStatus();
	//LOG4CPLUS_DEBUG(log, "m_readyState:" << *pVal);
	return S_OK;
}


STDMETHODIMP CATLWebSocket::get_onopen(IDispatch** pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_onopen;
	return S_OK;
}


STDMETHODIMP CATLWebSocket::put_onopen(IDispatch* newVal)
{
	// TODO: Add your implementation code here
	m_onopen = newVal;
	LOG4CPLUS_DEBUG(log, m_wsclient->m_SessionId << "set onopen:" << newVal);
	return S_OK;
}


STDMETHODIMP CATLWebSocket::get_onmessage(IDispatch** pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_onmessage;
	return S_OK;
}


STDMETHODIMP CATLWebSocket::put_onmessage(IDispatch* newVal)
{
	// TODO: Add your implementation code here
	m_onmessage = newVal;
	LOG4CPLUS_DEBUG(log, m_wsclient->m_SessionId << "set onmessag:" << newVal);
	return S_OK;
}


STDMETHODIMP CATLWebSocket::get_onclose(IDispatch** pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_onclose;
	return S_OK;
}


STDMETHODIMP CATLWebSocket::put_onclose(IDispatch* newVal)
{
	// TODO: Add your implementation code here
	m_onclose = newVal;
	LOG4CPLUS_DEBUG(log, m_wsclient->m_SessionId << "set onclose:" << newVal);
	return S_OK;
}


STDMETHODIMP CATLWebSocket::get_onerror(IDispatch** pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_onerror;
	return S_OK;
}


STDMETHODIMP CATLWebSocket::put_onerror(IDispatch* newVal)
{
	// TODO: Add your implementation code here
	m_onerror = newVal;
	LOG4CPLUS_DEBUG(log, m_wsclient->m_SessionId << "set onerror:" << newVal);
	return S_OK;
}


STDMETHODIMP CATLWebSocket::send(BSTR data)
{
	// TODO: Add your implementation code here
	if (data){
		std::string strdata = Unicode2Utf8(data);
		m_wsclient->Send(strdata.c_str(), strdata.length());
	}
	return S_OK;
}


STDMETHODIMP CATLWebSocket::close()
{
	// TODO: Add your implementation code here
	m_wsclient->Close();
	return S_OK;
}


STDMETHODIMP CATLWebSocket::get_onexception(IDispatch** pVal)
{
	// TODO: Add your implementation code here
	return get_onerror(pVal);
}


STDMETHODIMP CATLWebSocket::put_onexception(IDispatch* newVal)
{
	// TODO: Add your implementation code here
	return put_onerror(newVal);
}
