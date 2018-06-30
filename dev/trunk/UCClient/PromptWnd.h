#pragma once
#include "stdafx.h"

class CPromptWnd : public WindowImplBase
{

public:
	CPromptWnd();
	~CPromptWnd();

public:
	LPCTSTR GetWindowClassName() const { return _T("UIPromptWnd"); };
	//virtual void OnFinalMessage(HWND hWnd) {	WindowImplBase::OnFinalMessage(hWnd);	delete this;	};
	virtual CDuiString GetSkinFile() { return _T("prompt.xml"); };
	virtual CDuiString GetSkinFolder() { return  RESOURCE_PATH; };
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	void InitWindow();
	void Notify(TNotifyUI& msg);
	void OnPrepare(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);

};

