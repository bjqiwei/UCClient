#pragma once
#include "stdafx.h"
#include "websocket.h"
#include <thread>
#include <atomic>
#include <log4cplus/logger.h>

enum {
	MICROTEST_TIMER = 0,
	SPEAKERTEST_TIMER = 1,
};

class CSettingsWnd : public WindowImplBase
{

public:
	CSettingsWnd();
	~CSettingsWnd();

public:
	LPCTSTR GetWindowClassName() const { return _T("UISettingsWnd"); };
	//virtual void OnFinalMessage(HWND hWnd) {	WindowImplBase::OnFinalMessage(hWnd);	delete this;	};
	virtual CDuiString GetSkinFile() { return _T("settings.xml"); };
	virtual CDuiString GetSkinFolder() { return  RESOURCE_PATH; };
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	void InitWindow();
	void Notify(TNotifyUI& msg);
	void OnPrepare(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);

public:
	CEditUI* m_pPort;
	CEditUI* m_pLogLevel;
	CEditUI* m_pLogPath;
	CComboUI* m_pMicrophoneCombo;
	CCheckBoxUI* m_pIsMicrophone;
	CSliderUI* m_pMicrophoneVolume;
	CControlUI* m_pMicrophoneMute;
	CProgressUI* m_pMicrophoneProgress;
	CButtonUI* m_pMicrophoneTest;
	CButtonUI* m_pMicrophonePlay;
	CButtonUI* m_pMicrophonePause;
	CComboUI* m_pSpeakerCombo;
	CSliderUI* m_pSpeakerVolume;
	CControlUI* m_pSpeakerMute;
	CProgressUI* m_pSpeakerProgress;
	CButtonUI* m_pSpeakerTest;
	CButtonUI* m_pSpeakerPlay;
	CButtonUI* m_pSpeakerPause;

public:
	log4cplus::Logger log;
	CString  m_LogPath;
	uint16_t m_wsPort = 0;
	uint32_t m_SpeakerVolume = 0;
	uint32_t m_MicroVolume = 0;
	CString  m_SpeakerName;
	CString  m_MicroName;
	uint32_t m_LogLevel = 0;

};
