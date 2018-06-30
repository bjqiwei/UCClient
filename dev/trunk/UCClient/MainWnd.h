#pragma once
#include "stdafx.h"
#include <shellapi.h>
#include <tlhelp32.h>

#include <Windows.h>
#include <string>
#include <iostream>
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <ShlObj.h>
#include <DbgHelp.h>
#include <WtsApi32.h>
#include <signal.h>
#include "codingHelper.h"
#include "UCClientWebSocket.h"
#include <json/json.h>


class CMainWnd : public WindowImplBase
{

public:
	CMainWnd();
	~CMainWnd();

public:
	LPCTSTR GetWindowClassName() const { return _T("UIMainWnd"); };
	//virtual void OnFinalMessage(HWND hWnd) {	WindowImplBase::OnFinalMessage(hWnd);	delete this;	};
	virtual CDuiString GetSkinFile() { return _T("main.xml"); };
	virtual CDuiString GetSkinFolder() { return  RESOURCE_PATH; };
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnShowTask(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCloseWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	void InitWindow();
	void Notify(TNotifyUI& msg);
	void OnPrepare(TNotifyUI& msg);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	void OnSessionChange(UINT nSessionState, UINT nId);
	void MoveToDesktopRightBottom(HWND hWnd);
	bool IsProcessRun(char *pName);

private:
	NOTIFYICONDATA m_nid;
	CLabelUI* m_pStatusLabel;

public:
	void Start();
	void Stop();
	void WSStart();
	void WSStop();
	bool listenWS();

	uint16_t m_wsPort = 0;
	log4cplus::Logger log;

private:
	std::atomic_bool m_bWSRunning = false;
	std::thread m_wsserver;

	void signton();
public:
	static std::thread m_sington;
	static HANDLE m_SingletonEvent;
};