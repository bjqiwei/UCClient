#include "stdafx.h"
#include "MainWnd.h"
#include "SettingsWnd.h"
#include "PromptWnd.h"

std::thread CMainWnd::m_sington;
HANDLE CMainWnd::m_SingletonEvent = nullptr;
CMainWnd::CMainWnd()
{
	this->m_wsPort = ReadRegKeyDWORD("WebSocketPort", 19998);

	log = log4cplus::Logger::getInstance("CPjSipSDKBase");
	CWinSDKBase::initialize();
}

CMainWnd::~CMainWnd()
{
	CWinSDKBase::unInitialize();
}

CControlUI* CMainWnd::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

LRESULT CMainWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return 0;
}

LRESULT CMainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = 0;
	bHandled = TRUE;
	switch (uMsg) {
	case MESSAGE_SHOW_TASK:					lRes = OnShowTask(uMsg, wParam, lParam, bHandled); break;
	case MESSAGE_CLOSE_WINDOW:				lRes = OnCloseWindow(uMsg, wParam, lParam, bHandled); break;
	case WM_WTSSESSION_CHANGE:				OnSessionChange(wParam, lParam); break;
	default:
		bHandled = FALSE;	break;
	}

	if (lParam == WM_LBUTTONDOWN) {
		lRes = OnMouseLButtonDown(uMsg, wParam, lParam, bHandled); 
	}
	if (lParam == WM_RBUTTONDOWN) {
		lRes = OnMouseRButtonDown(uMsg, wParam, lParam, bHandled);
	}

	return 0;
}

void CMainWnd::InitWindow()
{
	m_pStatusLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("status")));
	if (!WTSRegisterSessionNotification(m_hWnd, NOTIFY_FOR_ALL_SESSIONS))
		LOG4CPLUS_DEBUG(log, "WTSRegisterSessionNotification:" << GetLastError());
}

void CMainWnd::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("windowinit")) == 0)
	{
		OnPrepare(msg);
	}
	else if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("closebtn") || msg.pSender->GetName() == _T("minbtn"))
		{
			/*CPromptWnd *pPromptWnd = new CPromptWnd();
			pPromptWnd->Create(this->GetHWND(), _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
			pPromptWnd->CenterWindow();
			pPromptWnd->ShowWindow(true);
			*/
			
			//SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
			memset(&m_nid, 0, sizeof(NOTIFYICONDATA));
			m_nid.cbSize = sizeof(NOTIFYICONDATA);
			m_nid.hIcon = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_UCCLIENT));
			m_nid.hWnd = m_hWnd;
			lstrcpy(m_nid.szTip, _T("软电话程序"));
			m_nid.uCallbackMessage = MESSAGE_SHOW_TASK;
			m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			Shell_NotifyIcon(NIM_ADD, &m_nid);
			ShowWindow(SW_HIDE);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("settingsbtn")) == 0)
		{
			CSettingsWnd *pSettingsWnd = new CSettingsWnd();
			pSettingsWnd->Create(NULL, _T("高级设置"), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
			pSettingsWnd->CenterWindow();
			pSettingsWnd->ShowModal();
			delete pSettingsWnd;
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("showlogbtn")) == 0)
		{
			CString sPath = ReadRegKeyString("LogPath");
			ShellExecute(NULL, "explore", sPath, NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

void CMainWnd::OnPrepare(TNotifyUI& msg)
{
	m_pStatusLabel->SetText("运行中");

	this->Start();
}

LRESULT CMainWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if (!WTSUnRegisterSessionNotification(m_hWnd))
		LOG4CPLUS_DEBUG(log, "WTSUnRegisterSessionNotification:" << GetLastError());

	this->Stop();
	PostQuitMessage(0);
	return WindowImplBase::OnClose(uMsg,wParam,lParam,bHandled);
}

void CMainWnd::MoveToDesktopRightBottom(HWND wnd)
{
	ASSERT(::IsWindow(wnd));
	ASSERT((GetWindowStyle(wnd)&WS_CHILD) == 0);
	RECT rcDlg = { 0 };
	::GetWindowRect(wnd, &rcDlg);
	RECT rcArea = { 0 };
	RECT rcCenter = { 0 };
	HWND hWnd = *this;
	HWND hWndParent = ::GetParent(wnd);
	HWND hWndCenter = ::GetWindowOwner(wnd);
	if (hWndCenter != NULL)
		hWnd = hWndCenter;

	// 处理多显示器模式下屏幕居中
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	rcArea = oMonitor.rcWork;

	if (hWndCenter == NULL)
		rcCenter = rcArea;
	else
		::GetWindowRect(hWndCenter, &rcCenter);

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter
	int xLeft = rcCenter.right - DlgWidth;
	int yTop = rcCenter.bottom - DlgHeight;

	// The dialog is outside the screen, move it inside
	if (xLeft < rcArea.left)
		xLeft = rcArea.left;
	else if (xLeft + DlgWidth > rcArea.right)
		xLeft = rcArea.right - DlgWidth;
	if (yTop < rcArea.top)
		yTop = rcArea.top;
	else if (yTop + DlgHeight > rcArea.bottom)
		yTop = rcArea.bottom - DlgHeight;
	::SetWindowPos(wnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

LRESULT CMainWnd::OnShowTask(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CMainWnd::OnCloseWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0);
	return 0;
}

LRESULT CMainWnd::OnMouseLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Shell_NotifyIcon(NIM_DELETE, &m_nid);//删除托盘图标
	ShowWindow(SW_SHOWNORMAL);//显示主窗口

	bHandled = FALSE;
	return 0;
}

LRESULT CMainWnd::OnMouseRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//获取鼠标坐标  
	POINT pt; 
	GetCursorPos(&pt);
	SetForegroundWindow(m_hWnd);	//右击后点别地可以清除“右击出来的菜单”

	HMENU hMenu;	//托盘菜单
	hMenu = CreatePopupMenu();	//生成托盘菜单   
	AppendMenu(hMenu, MF_STRING, MESSAGE_CLOSE_WINDOW, _T("退出"));

	int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, m_hWnd, NULL);
	if (cmd == MESSAGE_CLOSE_WINDOW)
	{
		//退出程序
		m_nid.hIcon = NULL;
		Shell_NotifyIcon(NIM_DELETE, &m_nid);
		this->Close();
	}

	bHandled = FALSE;
	return 0;
}

bool CMainWnd::IsProcessRun(char *pName)
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	bool bFind = false;
	// Take a snapshot of all processes in the system.  
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// Set the size of the structure before using it.  
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,  
	// and exit if unsuccessful  
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);          // clean the snapshot object  
		return false;
	}

	// Now walk the snapshot of processes, and  
	// display information about each process in turn  
	do
	{
		// Retrieve the priority class.  
		dwPriorityClass = 0;
		if (::strstr(pe32.szExeFile, pName) != NULL)
		{
			bFind = true;
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return bFind;
}


bool CMainWnd::listenWS()
{
	bool result = true;

	UCClientWSServer wsserver(this->m_wsPort);
	LOG4CPLUS_INFO(log, ",websocket start listen port:" << this->m_wsPort);
	wsserver.InitInstance();

	while (m_bWSRunning) {
		wsserver.Loop(1 * 1000);
	}

	wsserver.UnInitInstance();
	log4cplus::threadCleanup();
	return result;
}

void CMainWnd::Start()
{
	this->log = log4cplus::Logger::getInstance("UCClient");
	WSStart();
	m_sington = std::thread(&CMainWnd::signton, this);

}

void CMainWnd::Stop() {

	WSStop();
	//unInitialize();
	//CPjSipSDK::unInitialize();
}

void CMainWnd::WSStart()
{
	m_bWSRunning = true;
	if (!m_wsserver.joinable()) {
		m_wsserver = std::thread(&CMainWnd::listenWS, this);
	}
}

void CMainWnd::WSStop()
{
	this->m_bWSRunning = false;
	if (m_wsserver.joinable()) {
		m_wsserver.join();
	}
}


void CMainWnd::signton()
{
	LOG4CPLUS_DEBUG(log, "signton start.");

	while (WaitForSingleObject(m_SingletonEvent, INFINITE) == WAIT_OBJECT_0) {
		if (m_bWSRunning)
			this->ShowWindow();
		else
			break;
	}
	LOG4CPLUS_DEBUG(log, "signton stop");
}


void CMainWnd::OnSessionChange(UINT nSessionState, UINT nId)
{
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnWTSSessionChange";

	switch (nSessionState)
	{
	case WTS_CONSOLE_CONNECT:
		//LOG4CPLUS_DEBUG(log, "WTS_CONSOLE_CONNECT");
		out["param"]["sessionState"] = "CONSOLE_CONNECT";
		break;
	case WTS_CONSOLE_DISCONNECT:
		//LOG4CPLUS_DEBUG(log, "WTS_CONSOLE_DISCONNECT");
		out["param"]["sessionState"] = "CONSOLE_DISCONNECT";
		break;
	case WTS_REMOTE_CONNECT:
		//LOG4CPLUS_DEBUG(log, "WTS_REMOTE_CONNECT");
		out["param"]["sessionState"] = "REMOTE_CONNECT";
		break;
	case WTS_REMOTE_DISCONNECT:
		//LOG4CPLUS_DEBUG(log, "WTS_REMOTE_DISCONNECT");
		out["param"]["sessionState"] = "REMOTE_DISCONNECT";
		break;
	case WTS_SESSION_LOGON:
		//LOG4CPLUS_DEBUG(log, "WTS_SESSION_LOGON");
		out["param"]["sessionState"] = "SESSION_LOGON";
		break;
	case WTS_SESSION_LOGOFF:
		//LOG4CPLUS_DEBUG(log, "WTS_SESSION_LOGOFF");
		out["param"]["sessionState"] = "SESSION_LOGOFF";
		break;
	case WTS_SESSION_LOCK:
		//LOG4CPLUS_DEBUG(log, "WTS_SESSION_LOCK");
		out["param"]["sessionState"] = "SESSION_LOCK";
		break;
	case WTS_SESSION_UNLOCK:
		//LOG4CPLUS_DEBUG(log, "WTS_SESSION_UNLOCK");
		out["param"]["sessionState"] = "SESSION_UNLOCK";
		break;
	case WTS_SESSION_REMOTE_CONTROL:
		//LOG4CPLUS_DEBUG(log, "WTS_SESSION_REMOTE_CONTROL");
		out["param"]["sessionState"] = "SESSION_REMOTE_CONTROL";
		break;
	default:
		//LOG4CPLUS_ERROR(log, "Unkown Session State:" << wParam);
		break;
	}

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	return ;
}