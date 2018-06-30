#include "stdafx.h"
#include "PromptWnd.h"

CPromptWnd::CPromptWnd()
{
}

CPromptWnd::~CPromptWnd()
{
}

CControlUI* CPromptWnd::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

LRESULT CPromptWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return 0;
}

LRESULT CPromptWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = 0;
	bHandled = TRUE;
	switch (uMsg) {
	default:
		bHandled = FALSE;	break;
	}

	return 0;
}

void CPromptWnd::InitWindow()
{
}

void CPromptWnd::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("windowinit")) == 0)
	{
		OnPrepare(msg);
	}
	else if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), _T("closebtn")) == 0)
		{
			OnExit(msg);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("cancelbtn")) == 0)
		{
			OnExit(msg);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("okbtn")) == 0)
		{
			::PostMessage(GetParent(*this), MESSAGE_CLOSE_WINDOW, 0, 0);
		}
	}
}

void CPromptWnd::OnPrepare(TNotifyUI& msg)
{
}

void CPromptWnd::OnExit(TNotifyUI& msg)
{
	Close();
}