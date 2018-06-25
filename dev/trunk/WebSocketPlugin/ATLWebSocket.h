// ATLWebSocket.h : Declaration of the CATLWebSocket
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include <comutil.h>
#include "WebSocketPlugin_i.h"
#include "_IATLWebSocketEvents_CP.h"
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include "websocket/websocket.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;



// CATLWebSocket
class ATL_NO_VTABLE CATLWebSocket :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IATLWebSocket, &IID_IATLWebSocket, &LIBID_WebSocketPluginLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IOleControlImpl<CATLWebSocket>,
	public IOleObjectImpl<CATLWebSocket>,
	public IOleInPlaceActiveObjectImpl<CATLWebSocket>,
	public IViewObjectExImpl<CATLWebSocket>,
	public IOleInPlaceObjectWindowlessImpl<CATLWebSocket>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CATLWebSocket>,
	public CProxy_IATLWebSocketEvents<CATLWebSocket>,
	public IObjectWithSiteImpl<CATLWebSocket>,
	public IServiceProviderImpl<CATLWebSocket>,
	public IQuickActivateImpl<CATLWebSocket>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CATLWebSocket>,
#endif
	public IProvideClassInfo2Impl<&CLSID_ATLWebSocket, &__uuidof(_IATLWebSocketEvents), &LIBID_WebSocketPluginLib>,
	public IObjectSafetyImpl<CATLWebSocket, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
	public CComCoClass<CATLWebSocket, &CLSID_ATLWebSocket>,
	public CComCompositeControl<CATLWebSocket>
{
public:
	enum {
		WM_onOpen = WM_USER + 111,
		WM_onClose,
		WM_onError,
		WM_onMessage,
	};

	CATLWebSocket();
	~CATLWebSocket();
DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_ATLWEBSOCKET)


BEGIN_COM_MAP(CATLWebSocket)
	COM_INTERFACE_ENTRY(IATLWebSocket)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IQuickActivate)
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IServiceProvider)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_PROP_MAP(CATLWebSocket)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CATLWebSocket)
	CONNECTION_POINT_ENTRY(__uuidof(_IATLWebSocketEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CATLWebSocket)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER(WM_onOpen, onOpen)
	MESSAGE_HANDLER(WM_onMessage, onMessage)
	MESSAGE_HANDLER(WM_onClose, onClose)
	MESSAGE_HANDLER(WM_onError, onError)
	CHAIN_MSG_MAP(CComCompositeControl<CATLWebSocket>)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

BEGIN_SINK_MAP(CATLWebSocket)
	//Make sure the Event Handlers have __stdcall calling convention
END_SINK_MAP()

CComQIPtr<IHTMLDocument2> m_Document;
CComQIPtr<IDispatch> m_Script;
std::string m_UserAgent;

HRESULT get_Document(void ** pDoc){
	HRESULT hr = E_POINTER;
	if (this->m_spClientSite != NULL){

		CComQIPtr<IServiceProvider> pISP;
		if (FAILED(hr = m_spClientSite->QueryInterface(IID_IServiceProvider, (void **)&pISP))){
			LOG4CPLUS_ERROR(log, "get IServiceProvider");
		}
		else {
			CComQIPtr<IWebBrowser2> pIWebBrowser2;
			if (FAILED(hr = pISP->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void **)&pIWebBrowser2))){
				LOG4CPLUS_ERROR(log, "get WebBrowser2");
			}
			else
			{
				CComQIPtr<IDispatch> pqiDisp;
				if (FAILED(hr = pIWebBrowser2->get_Document(&pqiDisp))){
					LOG4CPLUS_ERROR(log, "get Document");
				}
				else{
					if (FAILED(hr = pqiDisp->QueryInterface(IID_IHTMLDocument2, pDoc))){
						LOG4CPLUS_ERROR(log, "get HTMLDocument");
					}
				}
			}
		}
	}
	return hr;
}

HRESULT get_userAgent(std::string & sUserAgent){
	
	HRESULT hr = E_POINTER;
	CComQIPtr<IHTMLWindow2>pWindow;
	if (m_Document == NULL)
	{
		get_Document((void **)&m_Document);
	}
	if (m_Document)
	{
		if (FAILED(hr = m_Document->get_parentWindow(&pWindow))){
			LOG4CPLUS_ERROR(log, "get parent Window");
		}
		else{
			CComQIPtr<IOmNavigator>pNavigator;
			if (FAILED(hr = pWindow->get_navigator(&pNavigator))){
				LOG4CPLUS_ERROR(log, "get Navigator");
			}
			else{
				BSTR  userAgent = NULL;
				if (SUCCEEDED(hr = pNavigator->get_userAgent(&userAgent)))
				{
					sUserAgent = (const char *)_bstr_t(userAgent);
					SysFreeString(userAgent);
				}
			}
		}
	}
	
	return hr;
}

HRESULT get_Script(IDispatch ** script){
	HRESULT hr = E_POINTER;
	if (m_Document == NULL)
	{
		get_Document((void **)&m_Document);
	}

	if (m_Document){
		hr = m_Document->get_Script(script);
	}

	return hr;
}

	STDMETHOD(OnAmbientPropertyChange)(DISPID dispid)
	{
		if (dispid == DISPID_AMBIENT_BACKCOLOR)
		{
			SetBackgroundColorFromAmbient();
			FireViewChange();
		}
		return IOleControlImpl<CATLWebSocket>::OnAmbientPropertyChange(dispid);
	}
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* const arr[] =
		{
			&IID_IATLWebSocket,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IATLWebSocket

	enum { IDD = IDD_ATLWEBSOCKET };
	STDMETHOD(_InternalQueryService)(REFGUID /*guidService*/, REFIID /*riid*/, void** /*ppvObject*/)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(SetClientSite)(IOleClientSite *pClientSite)
	{
		if (pClientSite)
		{
			RECT rc = { 0,0,0,0 };
			// Don't have access to the container's window so just use the
			// desktop. Window will be resized correctly during in-place
			// activation.
			HWND hWnd = CreateControlWindow(::GetDesktopWindow(), rc);
			_ASSERT(hWnd);
		}
		return IOleObjectImpl<CATLWebSocket>::SetClientSite(pClientSite);
	}

	HRESULT InPlaceActivate(LONG iVerb, const RECT* prcPosRect)
	{
		// Get the container's window.
		_ASSERT(m_spClientSite);
		LPOLEINPLACESITE pInPlaceSite = NULL;
		HRESULT hr = m_spClientSite->QueryInterface(IID_IOleInPlaceSite, (void**)&pInPlaceSite);
		_ASSERT(SUCCEEDED(hr) && pInPlaceSite);
		HWND hParent = NULL;
		hr = pInPlaceSite->GetWindow(&hParent);
		_ASSERT(SUCCEEDED(hr) && hParent);
		pInPlaceSite->Release();

		// Set container window as our parent window
		SetParent(hParent);
		return CComControlBase::InPlaceActivate(iVerb, prcPosRect);
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

private:
	log4cplus::Logger log;
	class ATLWSclient * m_wsclient = nullptr;
	CComDispatchDriver m_onopen = nullptr;
	CComDispatchDriver m_onmessage = nullptr;
	CComDispatchDriver m_onclose = nullptr;
	CComDispatchDriver m_onerror = nullptr;

public:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onOpen(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onError(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	STDMETHOD(setwsurl)(BSTR url);
	STDMETHOD(get_m_readyState)(LONG* pVal);
	STDMETHOD(get_onopen)(IDispatch** pVal);
	STDMETHOD(put_onopen)(IDispatch* newVal);
	STDMETHOD(get_onmessage)(IDispatch** pVal);
	STDMETHOD(put_onmessage)(IDispatch* newVal);
	STDMETHOD(get_onclose)(IDispatch** pVal);
	STDMETHOD(put_onclose)(IDispatch* newVal);
	STDMETHOD(get_onerror)(IDispatch** pVal);
	STDMETHOD(put_onerror)(IDispatch* newVal);
	STDMETHOD(send)(BSTR data);
	STDMETHOD(close)();
	STDMETHOD(get_onexception)(IDispatch** pVal);
	STDMETHOD(put_onexception)(IDispatch* newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(ATLWebSocket), CATLWebSocket)
