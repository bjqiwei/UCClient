#include "stdafx.h"
#include "SettingsWnd.h"
#include <log4cplus/loggingmacros.h>
#include <json/json.h>
#include "codingHelper.h"
#include <iostream>

static CSettingsWnd * configDlg = nullptr;

class CAudioMediaPlayer :public pj::AudioMediaPlayer{
public:
	CAudioMediaPlayer(CSettingsWnd * Wnd):m_Wnd(Wnd){
		this->log = log4cplus::Logger::getInstance("playfile");
	}
	~CAudioMediaPlayer() {

	}

	virtual bool onEof() override
	{
		m_Wnd->m_pSpeakerPlay->SetVisible(true);
		m_Wnd->m_pSpeakerPause->SetVisible(false);
		KillTimer(m_Wnd->GetHWND(), SPEAKERTEST_TIMER);
		try {
			pj::AudioMedia& play_med = pj::Endpoint::instance().audDevManager().getPlaybackDevMedia();
			this->stopTransmit(play_med);
		}
		catch (pj::Error& err)
		{
			LOG4CPLUS_ERROR(log, err.info());
		}
		return false;
	}
private:
	const CSettingsWnd * m_Wnd = nullptr;
	log4cplus::Logger log;
};

CSettingsWnd::CSettingsWnd()
{
	this->log = log4cplus::Logger::getInstance("WebCall");
}

CSettingsWnd::~CSettingsWnd()
{
	if(m_player)
		delete m_player;
}

CControlUI* CSettingsWnd::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

LRESULT CSettingsWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return 0;
}

LRESULT CSettingsWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = 0;
	bHandled = TRUE;
	switch (uMsg) {
	case WM_TIMER:								lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
	default:
		bHandled = FALSE;	break;
	}

	return 0;
}

void CSettingsWnd::InitWindow()
{
	m_pPort = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("port")));
	m_pLogLevel = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("logLevel")));
	m_pLogPath = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("logPath")));
	m_pMicrophoneCombo = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("microphone")));
	m_pIsMicrophone = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("ismicrophone")));
	m_pMicrophoneVolume = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("microphoneVolume")));
	m_pMicrophoneMute = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("microphoneMute")));
	m_pMicrophoneProgress = static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("microphoneProgress")));
	m_pMicrophoneTest = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("microphoneTest")));
	m_pMicrophonePlay = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("microphonePlay")));
	m_pMicrophonePause = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("microphonePause")));
	m_pSpeakerCombo = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("speaker")));
	m_pSpeakerVolume = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("speakerVolume")));
	m_pSpeakerMute = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("speakerMute")));
	m_pSpeakerProgress = static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("speakerProgress")));
	m_pSpeakerTest = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("speakerTest")));
	m_pSpeakerPlay = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("speakerPlay")));
	m_pSpeakerPause = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("speakerPause")));

	this->m_pSpeakerVolume->SetMinValue(0);
	this->m_pSpeakerVolume->SetMaxValue(100);

	this->m_pMicrophoneVolume->SetMinValue(0);
	this->m_pMicrophoneVolume->SetMaxValue(100);


	try {
		pj::Endpoint::instance().audDevManager().refreshDevs();
		const pj::AudioDevInfoVector & auddev = pj::Endpoint::instance().audDevManager().enumDev();
		for (int i = 0; i < auddev.size(); i++) {

			if (auddev[i]->inputCount)
			{
				CListLabelElementUI* pListLabelElementUI = new CListLabelElementUI();
				pListLabelElementUI->SetText(auddev[i]->name.c_str());
				m_pMicrophoneCombo->Add(pListLabelElementUI);
			}

			if (auddev[i]->outputCount)
			{
				CListLabelElementUI* pListLabelElementUI_Speaker = new CListLabelElementUI();
				pListLabelElementUI_Speaker->SetText(auddev[i]->name.c_str());
				m_pSpeakerCombo->Add(pListLabelElementUI_Speaker);
			}
		}
	}
	catch (pj::Error& err)
	{
		LOG4CPLUS_ERROR(log, err.info());
	}

	m_pMicrophoneCombo->SelectItem(0);
	m_pSpeakerCombo->SelectItem(0);

}

void CSettingsWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
	{
		OnPrepare(msg);
	}
	else if (msg.sType == DUI_MSGTYPE_VALUECHANGED)
	{
		if (msg.pSender->GetName() == _T("microphoneVolume")) {
			try {
				pj::Endpoint::instance().audDevManager().getCaptureDevMedia().adjustTxLevel(this->m_pMicrophoneVolume->GetValue()/50.0);
			}
			catch (pj::Error& err)
			{
				LOG4CPLUS_ERROR(log, err.info());
			}
		}

		else if (msg.pSender->GetName() == _T("speakerVolume")) {
			try {
				pj::Endpoint::instance().audDevManager().getPlaybackDevMedia().adjustRxLevel(this->m_pSpeakerVolume->GetValue()/50.0);
			}
			catch (pj::Error& err)
			{
				LOG4CPLUS_ERROR(log, err.info());
			}
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
	{
		if (msg.pSender->GetName() == _T("microphone")) {

			int micro = m_pMicrophoneCombo->GetCurSel();
			int microindex = 0;

			try {
				const pj::AudioDevInfoVector & auddev = pj::Endpoint::instance().audDevManager().enumDev();
				for (int i = 0; i < auddev.size(); i++) {

					if (auddev[i]->inputCount && micro == microindex++) {
						pj::Endpoint::instance().audDevManager().setCaptureDev(i);
					}
				}

			}
			catch (pj::Error& err)
			{
				LOG4CPLUS_ERROR(log, err.info());
			}
		}
		if (msg.pSender->GetName() == _T("speaker")) {
			
			int speaker = m_pSpeakerCombo->GetCurSel();
			int speakerindex = 0;

			try {
				const pj::AudioDevInfoVector & auddev = pj::Endpoint::instance().audDevManager().enumDev();
				for (int i = 0; i < auddev.size(); i++) {

					if (auddev[i]->outputCount && speaker == speakerindex++)
					{
						pj::Endpoint::instance().audDevManager().setPlaybackDev(i);
					}
				}
			}
			catch (pj::Error& err)
			{
				LOG4CPLUS_ERROR(log, err.info());
			}
		}
	}
	else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED)
	{
		if (msg.pSender->GetName() == "logPath") {
			//m_LogPath = m_pLogPath->GetText().GetData();
		}
		else if (msg.pSender->GetName() == _T("logLevel"))
		{
			
		}

	}
	else if (msg.sType == _T("click"))
	{
		if (msg.pSender->GetName() ==  _T("closebtn"))
		{
			OnExit(msg);
		}
		else if (msg.pSender->GetName() ==  _T("minbtn"))
		{
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
		else if (msg.pSender->GetName() ==  _T("backBtn"))
		{
			OnExit(msg);
		}
		else if (msg.pSender->GetName() == _T("speakerPlay"))
		{
			std::string utf8RingFile = utf8Dir + "ring.wav";
			try {
				pj::AudioMedia& play_med = pj::Endpoint::instance().audDevManager().getPlaybackDevMedia();
				if (this->m_player)
					delete m_player;

				m_player = new CAudioMediaPlayer(this);
				m_player->createPlayer(utf8RingFile);
				m_player->startTransmit(play_med);
				SetTimer(this->GetHWND(), SPEAKERTEST_TIMER, 100, nullptr);
				this->m_pSpeakerPlay->SetVisible(false);
				this->m_pSpeakerPause->SetVisible(true);
			}
			catch (pj::Error& err)
			{
				LOG4CPLUS_ERROR(log, "Error play ringfile :" << err.info());
			}
		}
		else if (msg.pSender->GetName() == _T("speakerPause"))
		{
			this->m_pSpeakerPlay->SetVisible(true);
			this->m_pSpeakerPause->SetVisible(false);
			KillTimer(this->GetHWND(), SPEAKERTEST_TIMER);
			try {
				pj::AudioMedia& play_med = pj::Endpoint::instance().audDevManager().getPlaybackDevMedia();
				if (this->m_player)
					m_player->stopTransmit(play_med);
			}
			catch (pj::Error& err)
			{
				LOG4CPLUS_ERROR(log, "Error play ringfile :" << err.info());
			}
		}

		else if (msg.pSender->GetName() == _T("microphonePlay"))
		{
			SetTimer(this->GetHWND(), MICROTEST_TIMER, 100, nullptr);
			this->m_pMicrophonePlay->SetVisible(false);
			this->m_pMicrophonePause->SetVisible(true);
			try {
				pj::AudioMedia& play_med = pj::Endpoint::instance().audDevManager().getPlaybackDevMedia();
				pj::AudioMedia& speaker_med = pj::Endpoint::instance().audDevManager().getCaptureDevMedia();
				speaker_med.startTransmit(play_med);
			}
			catch (pj::Error& err)
			{
				LOG4CPLUS_ERROR(log, err.info());
			}

		}
		else if (msg.pSender->GetName() == _T("microphonePause"))
		{
			this->m_pMicrophonePlay->SetVisible(true);
			this->m_pMicrophonePause->SetVisible(false);
			KillTimer(this->GetHWND(), MICROTEST_TIMER);
			try {
				pj::AudioMedia& play_med = pj::Endpoint::instance().audDevManager().getPlaybackDevMedia();
				pj::AudioMedia& speaker_med = pj::Endpoint::instance().audDevManager().getCaptureDevMedia();
				speaker_med.stopTransmit(play_med);
			}
			catch (pj::Error& err)
			{
				LOG4CPLUS_ERROR(log, err.info());
			}
		}
		
	}
}

void CSettingsWnd::OnPrepare(TNotifyUI& msg)
{

	this->m_wsPort = ReadRegKeyDWORD("WebSocketPort", 19996);
	this->m_SpeakerName = ReadRegKeyString("Speaker");
	this->m_MicroName = ReadRegKeyString("MicroPhone");
	this->m_SpeakerVolume = ReadRegKeyDWORD("SpeakerVolume", 50);
	this->m_MicroVolume = ReadRegKeyDWORD("MicroVolume", 50);
	this->m_LogLevel = ReadRegKeyDWORD("LogLevel", 23);
	this->m_LogPath = ReadRegKeyString("LogPath", "");

	CString sPort;
	sPort.Format(_T("%d"), this->m_wsPort);
	m_pPort->SetText(sPort);

	m_pLogPath->SetText(m_LogPath);

	for (int i = 0; i < m_pMicrophoneCombo->GetCount(); i++)
	{
		DuiLib::CControlUI * pControl = m_pMicrophoneCombo->GetItemAt(i);
		if (pControl && pControl->GetText() == m_MicroName.GetBuffer())
			m_pMicrophoneCombo->SelectItem(i);
	}
	m_pMicrophoneVolume->SetValue(m_MicroVolume);

	for (int i = 0; i < m_pSpeakerCombo->GetCount(); i++)
	{
		DuiLib::CControlUI * pControl = m_pSpeakerCombo->GetItemAt(i);
		if (pControl && pControl->GetText() == m_SpeakerName.GetBuffer())
			m_pSpeakerCombo->SelectItem(i);
	}
	m_pSpeakerVolume->SetValue(m_SpeakerVolume);
	CString sLogLevel;
	sLogLevel.Format(_T("%d"), m_LogLevel);
	m_pLogLevel->SetText(sLogLevel);

}

void CSettingsWnd::OnExit(TNotifyUI& msg)
{
			
	try
	{
		char * stop = nullptr;
		uint16_t port = std::strtol(m_pPort->GetText().GetData(), &stop, 10);
		if (port != this->m_wsPort) {
			SetRegKey("WebSocketPort", port);
		}
	}
	catch (std::exception* e)
	{
	}
			
	if (m_pSpeakerCombo->GetText() != this->m_SpeakerName.GetBuffer()) {
		SetRegKey("Speaker", m_pSpeakerCombo->GetText().GetData());
	}
	if (m_pMicrophoneCombo->GetText() != this->m_MicroName.GetBuffer()) {
		SetRegKey("MicroPhone", m_pMicrophoneCombo->GetText().GetData());
	}
	if (this->m_pSpeakerVolume->GetValue() != this->m_SpeakerVolume) {
		SetRegKey("SpeakerVolume", this->m_pSpeakerVolume->GetValue());
	}
	if (this->m_pMicrophoneVolume->GetValue() != this->m_MicroVolume) {
		SetRegKey("MicroVolume", this->m_pMicrophoneVolume->GetValue());
	}

	try
	{
		char * stop = nullptr;
		uint32_t loglevel = std::strtol(m_pLogLevel->GetText().GetData(), &stop, 10);
		if (loglevel != this->m_LogLevel) {
			SetRegKey("LogLevel", loglevel);
		}
	}
	catch (std::exception* e)
	{
	}
	
	if (m_pLogPath->GetText().GetData() != this->m_LogPath) {
		SetRegKey("LogPath", m_pLogPath->GetText().GetData());
	}

	try {
		pj::AudioMedia& play_med = pj::Endpoint::instance().audDevManager().getPlaybackDevMedia();
		pj::AudioMedia& speaker_med = pj::Endpoint::instance().audDevManager().getCaptureDevMedia();
		speaker_med.stopTransmit(play_med);
	}
	catch (pj::Error& err)
	{
		LOG4CPLUS_ERROR(log, err.info());
	}
	Close();
}

LRESULT CSettingsWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	switch (wParam) {
	case MICROTEST_TIMER: {
		m_pMicrophoneProgress->SetValue(pj::Endpoint::instance().audDevManager().getCaptureDevMedia().getTxLevel());
		bHandled = TRUE;
	}
	break;
	case SPEAKERTEST_TIMER: {
		m_pSpeakerProgress->SetValue(pj::Endpoint::instance().audDevManager().getPlaybackDevMedia().getRxLevel());
		bHandled = TRUE;
	}
	break;
	default:
		break;
	}
	return 0;
}

