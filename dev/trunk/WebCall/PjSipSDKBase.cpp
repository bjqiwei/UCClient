#include "stdafx.h"
#include "PjSipSDKBase.h"
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loggingmacros.h>
#include <atomic>
#include <map>
#include <mutex>
#include "codingHelper.h"

#pragma comment(lib,"version.lib")

static std::atomic_ulong g_pjsipReferce = 0;
class CPEndpoint * CPjSipSDKBase::ep;

class CPEndpoint : public pj::Endpoint {
public:
	CPEndpoint(){
		this->log = log4cplus::Logger::getInstance("pjsip");
	}

	virtual void onNatDetectionComplete(const pj::OnNatDetectionCompleteParam &prm) override
	{
		LOG4CPLUS_DEBUG(log, "nat detect " << prm.natTypeName << ", status:" << prm.reason);
	}

	pj::TransportId transport_tcp = PJSUA_INVALID_ID;
private:
	log4cplus::Logger log;

};


class CPCall : public pj::Call
{
public:
	CPCall(CAccount * acc, int call_id = PJSUA_INVALID_ID)
		: Call(*acc, call_id), m_acc(acc)
	{
		this->log = log4cplus::Logger::getInstance("CPCall");
		LOG4CPLUS_DEBUG(log, "New Call");
		
		CString micro = ReadRegKeyString("MicroPhone");
		CString speaker = ReadRegKeyString("Speaker");

		try {
			const pj::AudioDevInfoVector & auddev = pj::Endpoint::instance().audDevManager().enumDev();
			for (int i = 0; i < auddev.size(); i++) {

				if (auddev[i]->inputCount && micro == auddev[i]->name.c_str()) {
					pj::Endpoint::instance().audDevManager().setCaptureDev(i);
				}

				if (auddev[i]->outputCount && speaker == auddev[i]->name.c_str())
				{
					pj::Endpoint::instance().audDevManager().setPlaybackDev(i);
				}
			}

			pj::Endpoint::instance().audDevManager().getCaptureDevMedia().adjustTxLevel(ReadRegKeyDWORD("MicroVolume", 50) / (50.0));
			pj::Endpoint::instance().audDevManager().getPlaybackDevMedia().adjustRxLevel(ReadRegKeyDWORD("SpeakerVolume", 50)/(50.0));
		}
		catch (pj::Error& err)
		{
			LOG4CPLUS_ERROR(log, err.info());
		}

	}

	virtual ~CPCall()
	{ 
		LOG4CPLUS_DEBUG(log, "Release Call");
	}

	// Notification when call's state has changed.
	virtual void onCallState(pj::OnCallStateParam &prm) override
	{
		pj::CallInfo ci = getInfo();
		
		m_acc->onCallState(ci);
		if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
			/* Delete the call */
			std::recursive_mutex & mtx = m_acc->m_callsmtx;
			mtx.lock();
			m_acc->m_calls.erase(this->getId());
			delete this;
			mtx.unlock();
		}
	}

	// Notification when call's media state has changed.
	virtual void onCallMediaState(pj::OnCallMediaStateParam &prm) override
	{
		LOG4CPLUS_DEBUG(log, "hasMedia:" << this->hasMedia());
		for (auto & media : this->getInfo().media){
			if (media.type == PJMEDIA_TYPE_AUDIO) {
				pj::AudioMedia *aud_med = (pj::AudioMedia *) this->getMedia(media.index);
				pj::AudioMedia& speaker_med = pj::Endpoint::instance().audDevManager().getPlaybackDevMedia();
				aud_med->startTransmit(speaker_med);
				pj::AudioMedia& mic_med = pj::Endpoint::instance().audDevManager().getCaptureDevMedia();
				mic_med.startTransmit(*aud_med);

				if (m_micro.getPortId() == PJSUA_INVALID_ID) {
					m_micro.createRecorder(utf8AppDataDir + "\\micro.wav");
					mic_med.startTransmit(m_micro);
				}

				if (m_speaker.getPortId() == PJSUA_INVALID_ID){
					m_speaker.createRecorder(utf8AppDataDir + "\\speaker.wav");
					aud_med->startTransmit(m_speaker);
				}
			}
		}
	}

	virtual void onDtmfDigit(pj::OnDtmfDigitParam &prm) override
	{
		m_acc->onDtmfDigit(getInfo().id, prm.digit);
	}

	virtual void onCallTransferStatus(pj::OnCallTransferStatusParam &prm) override
	{
	}

private:
	CAccount * m_acc = nullptr;
	pj::AudioMediaRecorder m_micro;
	pj::AudioMediaRecorder m_speaker;
	log4cplus::Logger log;
};


void CPjSipSDKBase::onRegStarted(pj::OnRegStartedParam & prm)
{
	if (prm.renew){
		this->onConnected();
	}
	else{ 
		this->onLogOut();
	}
}

void CPjSipSDKBase::onRegState(pj::OnRegStateParam &prm)
{
	if (prm.code == 200) {
		//this->onConnected();
	}
	else {
		this->onConnectError(prm.code, prm.reason.c_str());
	}
}

void CPjSipSDKBase::onCallState(const pj::CallInfo & ci)
{
	LOG4CPLUS_DEBUG(log, "acc_id:" << ci.accId << ", Call " << ci.id << " state=" << ci.stateText);
	this->m_callid = ci.id;

	switch (ci.state) {
	case PJSIP_INV_STATE_CALLING:
		this->onCallProceeding(std::to_string(ci.id).c_str());
		break;
	case PJSIP_INV_STATE_INCOMING: {
		startRinging();
		std::string remote = ci.remoteUri;
		std::string caller = remote.substr(remote.find(":"), remote.find("@") - remote.find(":"));
		this->onIncomingCallReceived(0, std::to_string(ci.id).c_str(), caller.c_str());
		}
		break;
	case PJSIP_INV_STATE_EARLY:
		if(ci.role == PJSIP_UAC_ROLE) 
			this->onCallAlerting(std::to_string(ci.id).c_str());
		break;
	case PJSIP_INV_STATE_CONNECTING:
		//g_sipAccount[ci.acc_id]->onCallAlerting(std::to_string(call_id).c_str());
		break;
	case PJSIP_INV_STATE_CONFIRMED:
		stopRinging();
		this->onCallAnswered(std::to_string(ci.id).c_str());
		break;
	case PJSIP_INV_STATE_DISCONNECTED:
		stopRinging();
		this->onCallReleased(std::to_string(ci.id).c_str(), ci.lastStatusCode);
		break;
	}
	
}

void CPjSipSDKBase::onDtmfDigit(pjsua_call_id call_id, const std::string & digit)
{
	this->onDtmfReceived(std::to_string(call_id).c_str(), digit.back());
}


void CPjSipSDKBase::onIncomingCall(pj::Call * call)
{
	LOG4CPLUS_DEBUG(log, "Incoming call from " << call->getId());

	this->m_callid = call->getId();
	
	std::string remote = call->getInfo().remoteUri;
	std::string caller = remote.substr(remote.find(":"), remote.find("@")- remote.find(":"));
	startRinging();
	this->onIncomingCallReceived(0, std::to_string(call->getInfo().id).c_str(), caller.c_str());
}

class CPLogWriter :public pj::LogWriter {
private:
	log4cplus::Logger log;
public:
	CPLogWriter(){
		log = log4cplus::Logger::getInstance("pjsip.log");
	}

	virtual void write(const pj::LogEntry &entry)override
	{ 
		std::string msg = entry.msg; 
		msg.pop_back();
		if (entry.level == 5) {
			LOG4CPLUS_TRACE(log, msg);
		}
		else if (entry.level == 4) {
			LOG4CPLUS_DEBUG(log, msg);
		}
		else if (entry.level == 3) {
			LOG4CPLUS_INFO(log, msg);
		}
		else if (entry.level == 2) {
			LOG4CPLUS_WARN(log, msg);
		}
		else if (entry.level == 1) {
			LOG4CPLUS_ERROR(log, msg);
		}
		else
			LOG4CPLUS_TRACE(log, msg);
	}
};


std::string CPjSipSDKBase::getVersion()
{
	static std::string result;
	if (!result.empty())
		return result;

	result = "0.0.0.0";
	VS_FIXEDFILEINFO *pVerInfo = NULL;
	DWORD dwTemp, dwSize, dwHandle = 0;
	BYTE *pData = NULL;
	UINT uLen;
	std::wstring wstrModuleFileName = Utf82Unicode(utf8ModuleFilename);
	dwSize = GetFileVersionInfoSizeW(wstrModuleFileName.c_str(), &dwTemp);

	pData = new BYTE[dwSize];

	GetFileVersionInfoW(wstrModuleFileName.c_str(), dwHandle, dwSize, pData);
	VerQueryValue(pData, "\\", (void **)&pVerInfo, &uLen);


	DWORD verMS = pVerInfo->dwFileVersionMS;
	DWORD verLS = pVerInfo->dwFileVersionLS;

	int ver[4];
	ver[0] = HIWORD(verMS);
	ver[1] = LOWORD(verMS);
	ver[2] = HIWORD(verLS);
	ver[3] = LOWORD(verLS);

	std::stringstream ss;
	ss << ver[0] << "." << ver[1] << "." << ver[2] << "." << ver[3];
	result = ss.str();

	delete[] pData;
	return result;
}

CPjSipSDKBase::CPjSipSDKBase()
{
	log = log4cplus::Logger::getInstance("CPjSipSDKBase");
	LOG4CPLUS_INFO(log, "SDK Version:" << getVersion());
	LOG4CPLUS_INFO(log, "SDK:" << utf8ModuleFilename);

	initialize();
	
	if (!pj::Endpoint::instance().libIsThreadRegistered())
		pj::Endpoint::instance().libRegisterThread("CPjSipSDKBase");

	m_acc = new CAccount(this);

	LOG4CPLUS_TRACE(log, "construction");
}

CPjSipSDKBase::~CPjSipSDKBase()
{
	//pjsua_acc_set_user_data(m_acc->getId(), NULL);
	delete m_acc;
	if (m_player)
		delete m_player;
	unInitialize();
	LOG4CPLUS_TRACE(log, "destruction");
}

int CPjSipSDKBase::initialize()
{
	if (g_pjsipReferce.fetch_add(1) == 0) {

		log4cplus::Logger log = log4cplus::Logger::getInstance("pjsip");
		ep = new CPEndpoint();

		// pj create
		pj::EpConfig ep_cfg;
		ep_cfg.logConfig.level = 5;
		ep_cfg.logConfig.writer = new CPLogWriter();

		ep_cfg.medConfig.noVad = true;
		//ep_cfg.medConfig.sndClockRate = 16000;

		try {
			ep->libCreate();
			ep->libInit(ep_cfg);
			pj::TransportConfig tcfg;
			ep->transport_tcp = ep->transportCreate(PJSIP_TRANSPORT_TCP, tcfg);

			// Start the library (worker threads etc)
			ep->libStart();

			for (auto it : ep->codecEnum()) {
				LOG4CPLUS_DEBUG(log, it->codecId << " priority:" << (uint32_t)it->priority << ",desc:" << it->desc);
			}

			//ep->natDetectType();

			for (auto audiodev : ep->audDevManager().enumDev()) {
				LOG4CPLUS_DEBUG(log, "audiodevice:" << (audiodev->name));
			}
			pj::Endpoint::instance().audDevManager().setEcOptions(ep->audDevManager().getEcTail(), PJMEDIA_ECHO_WEBRTC | PJMEDIA_ECHO_USE_NOISE_SUPPRESSOR);
			LOG4CPLUS_DEBUG(log, "EcTail:" << ep->audDevManager().getEcTail());
		}
		catch (pj::Error &err){
			LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
			return err.status;
		}

	}

	return 0;
}

void CPjSipSDKBase::setRingFile(const std::string & ringfile)
{
	this->m_ringFile = ringfile;
	LOG4CPLUS_DEBUG(log, "setRingFile:" << this->m_ringFile);
}

void CPjSipSDKBase::startRinging()
{
	pj::AudioMedia& play_med = ep->audDevManager().getPlaybackDevMedia();
	try{
		if (this->m_player == nullptr) {
			this->m_player = new pj::AudioMediaPlayer();

			this->m_player->createPlayer(m_ringFile);
			this->m_player->startTransmit(play_med);
		}
	}
	catch (pj::Error& err)
	{
		LOG4CPLUS_ERROR(log, "Error play ringfile :" << err.info());
	}

}

void CPjSipSDKBase::stopRinging()
{
	pj::AudioMedia& play_med = ep->audDevManager().getPlaybackDevMedia();

	try
	{
		if (m_player && m_player->getPortId() != PJSUA_INVALID_ID) {
			m_player->stopTransmit(play_med);
			delete m_player;
			m_player = nullptr;
		}
	}
	catch (pj::Error& err)
	{
		LOG4CPLUS_ERROR(log, "Error stop play ringfile :" << err.info());
	}
}

int CPjSipSDKBase::connectToCCP(std::string utf8Addr, LONG port, std::string utf8voipId, std::string utf8voipPwd)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " addr:" << utf8Addr << ", port:" << port << ",voipId:" << utf8voipId << ", pwd:" << utf8voipPwd);

	this->m_addr = utf8Addr;
	this->port = port;
	this->m_voipid = utf8voipId;
	this->m_voippwd = utf8voipPwd;

	//����˺�
	pj::AccountConfig acc_cfg;
	acc_cfg.idUri = "<sip:" + this->m_voipid + "@" + this->m_addr + ":" + std::to_string(this->port) + ";transport=tcp>";
	acc_cfg.regConfig.registrarUri = "<sip:" + this->m_addr +":" + std::to_string(this->port) + ";transport=tcp>";
	acc_cfg.sipConfig.authCreds.push_back(pj::AuthCredInfo("Digest", "*", this->m_voipid, 0, this->m_voippwd));
	acc_cfg.sipConfig.transportId = ep->transport_tcp;

	try {
		if (m_acc->isValid()) {
			pjsua_acc_set_user_data(m_acc->getId(), nullptr);
			pjsua_acc_del(m_acc->getId());
		}

		this->m_acc->create(acc_cfg);
		//this->setRegistration(true);
		this->log = log4cplus::Logger::getInstance("CPjSipSDKBase." + std::to_string(this->m_acc->getId()));
	}
	catch (pj::Error& err) {
		LOG4CPLUS_ERROR(log, "Account creation error: " << err.info());
	}

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:");
	return 0;
}

std::string CPjSipSDKBase::makeCall(std::string strCalled) {

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " called:" << strCalled);
	
	if (ep->utilVerifySipUri(strCalled) != PJ_SUCCESS){
		strCalled = "sip:" + strCalled + "@" + this->m_addr + ":" + std::to_string(this->port);
	}

	pj::Call * call = nullptr;
	try {
		m_acc->makeCall(strCalled, &call);
	}
	catch (pj::Error &err) {
		this->onMakeCallFailed("", err.status);
		LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
		return "";
	}
	
	m_callid = call->getId();

	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << m_callid);
	return std::to_string(m_callid);
}

int CPjSipSDKBase::acceptCall(int callid) {
	pj::Call * call = pj::Call::lookup(callid);
	pj_status_t status = PJ_SUCCESS;
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " callid:" << callid);

	try {
		if (call && (call->getInfo().state == PJSIP_INV_STATE_INCOMING || call->getInfo().state == PJSIP_INV_STATE_EARLY))
		{
			pj::CallOpParam cprm;
			cprm.statusCode = PJSIP_SC_OK;
			cprm.opt.audioCount = 1;
			call->answer(cprm);
		}
	}
	catch (pj::Error &err) {
		LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
		status = err.status;
	}

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << status);
	return status;
}

int CPjSipSDKBase::rejectCall(int callid, int reason) {
	pj::Call * call = pj::Call::lookup(callid);
	pj_status_t status = PJ_SUCCESS;
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callid << ", reason:" << reason);
	try {
		if (call) {
			call->hangup(true);
		}
	}
	catch (pj::Error &err) {
		LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
		status = err.status;
	}
	LOG4CPLUS_DEBUG(log, __FUNCTION__" result:" << status);
	return status;
}

int CPjSipSDKBase::pauseCall(int callid)
{
	pj::Call * call = pj::Call::lookup(callid);
	pj_status_t status = PJ_SUCCESS;

	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callid);
	try {
		if (call) {
			call->setHold(true);
		}
	}
	catch (pj::Error &err) {
		LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
		status = err.status;
	}
	LOG4CPLUS_DEBUG(log, __FUNCTION__" result:" << status);
	if (status == PJ_SUCCESS){
		this->onCallPaused(std::to_string(callid).c_str());
	}
	return status;
}

int CPjSipSDKBase::resumeCall(int callid)
{
	pj::Call * call = pj::Call::lookup(callid);
	pj_status_t status = PJ_SUCCESS;

	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callid);
	try {
		if (call) {
			pj::CallOpParam rpm(true);
			rpm.opt.flag = PJSUA_CALL_UNHOLD;
			call->reinvite(rpm);
		}
	}
	catch (pj::Error &err) {
		LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
		status = err.status;
	}
	LOG4CPLUS_DEBUG(log, __FUNCTION__" result:" << status);
	if (status == PJ_SUCCESS) {
		this->onResumed(std::to_string(callid).c_str());
	}
	return status;
}

int CPjSipSDKBase::transferCall(int callid, std::string number, int type)
{
	pj::Call * call = pj::Call::lookup(callid);
	pj_status_t status = PJ_SUCCESS;

	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callid << ",dest:" << number);
	if (ep->utilVerifySipUri(number) != PJ_SUCCESS) {
		number = "sip:" + number + "@" + this->m_addr + ":" + std::to_string(this->port);
	}

	try {
		if (call) {
			call->xfer(number, true);
		}
	}
	catch (pj::Error &err) {
		LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
		status = err.status;
	}

	LOG4CPLUS_DEBUG(log, __FUNCTION__" result:" << status);

	if (status == PJ_SUCCESS)
		this->onCallTransfered(std::to_string(callid).c_str(), number.c_str());

	return status;
}

int CPjSipSDKBase::sendDTMF(int callid, const char dtmf)
{
	pj::Call * call = pj::Call::lookup(callid);
	pj_status_t status = PJ_SUCCESS;

	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callid << ",dtmf:" << dtmf);
	try {
		if (call) {
			std::string digit;
			digit.push_back(dtmf);
			call->dialDtmf(digit);
		}
	}
	catch (pj::Error &err) {
		LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
		status = err.status;
	}

	LOG4CPLUS_DEBUG(log, __FUNCTION__" result:" << status);
	return status;
}

int CPjSipSDKBase::releaseCall(int callid) {

	pj_status_t status = PJ_SUCCESS;
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callid);

	if (callid < 0){
		this->m_acc->m_callsmtx.lock();
		for (auto & call : this->m_acc->m_calls)
		{
			delete call.second;
		}
		this->m_acc->m_calls.clear();
		this->m_acc->m_callsmtx.unlock();
	}
	else {
		pj::Call * call = pj::Call::lookup(callid);

		try {
			if (call) {
				call->hangup(true);
			}
		}
		catch (pj::Error &err) {
			LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
			status = err.status;
		}
	}

	LOG4CPLUS_DEBUG(log, __FUNCTION__" result:" << status);
	return status;
}

const int CPjSipSDKBase::getCurrentCall() const
{
	return m_callid;
}

int CPjSipSDKBase::disConnectToCCP()
{
	pj_status_t status = PJ_SUCCESS;
	try
	{
		if (this->m_acc->isValid()) {
			this->m_acc->setRegistration(false);
		}
	}
	catch (pj::Error &err) {
		LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
		status = err.status;
	}

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << status);
	return status;
}

int CPjSipSDKBase::unInitialize()
{
	if (g_pjsipReferce.fetch_sub(1) == 1) {
		ep->hangupAllCalls();
		ep->libDestroy(pjsua_destroy_flag::PJSUA_DESTROY_NO_RX_MSG);
		//LOG4CPLUS_DEBUG(log, __FUNCTION__ );
		delete ep;
		ep = nullptr;
	}
	return 0;
}

int CPjSipSDKBase::setCodecEnabled(int codecid, int enabled)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" codec:" << codecid << ",enabled :" << enabled);

	std::string strcodec_id = getCodecs(codecid);

	pj_status_t status = PJ_SUCCESS;
	try {
		ep->codecSetPriority(strcodec_id, enabled ? PJMEDIA_CODEC_PRIO_NORMAL : PJMEDIA_CODEC_PRIO_DISABLED);
	}
	catch (pj::Error &err) {
		LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
		status = err.status;
	}
	
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << status);
	return status;
}

int CPjSipSDKBase::getCodecEnabled(int codecid)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" codec:" << codecid);

	std::string strcodec_id = getCodecs(codecid);

	pj_status_t status = PJ_SUCCESS;
	try {
		
		for (auto it : ep->codecEnum()) {
			if (it->codecId == strcodec_id) {
				status = it->priority != PJMEDIA_CODEC_PRIO_DISABLED;
				LOG4CPLUS_DEBUG(log, it->codecId << " priority:" << (uint8_t)it->priority << ",desc:" << it->desc);
			}
		}
	}
	catch (pj::Error &err) {
		LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
		status = 0;
	}

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << status);
	return status;
}

int CPjSipSDKBase::setMute(bool on)
{
	pj_status_t status = PJ_SUCCESS;
	LOG4CPLUS_DEBUG(log, __FUNCTION__" on:" << on);
	on = !on;
	try {
		ep->audDevManager().getCaptureDevMedia().adjustTxLevel((float)on / 100);
	}
	catch (pj::Error &err) {
		LOG4CPLUS_ERROR(log, err.info() << ":" << err.reason << ";" << err.srcFile << ":" << err.srcLine);
		status = err.status;
	}

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << status);
	return 0;
}

std::string CPjSipSDKBase::getCodecs(int type)
{
	std::string ret = "";
	switch (type)
	{
	case 0:	ret = "opus/16000/1";	break;	//Opus 16 kHz
	case 1:	ret = "opus/24000/1";	break;	//Opus 24 kHz
	case 2:	ret = "opus/48000/1";	break;	//Opus 48 kHz
	case 3:	ret = "PCMA/8000/1";	break;	//G.711 A-law
	case 4:	ret = "PCMU/8000/1";	break;	//G.711 u-law
	case 5:	ret = "G722/16000/1";	break;	//G.722 16 kHz
	case 6:	ret = "G729/8000/1";	break;	//G.729 8 kHz
	case 7:	ret = "GSM/8000/1";		break;	//GSM 8 kHz
	case 8:	ret = "AMR/8000/1";		break;	//AMR 8 kHz
	case 9:	ret = "iLBC/8000/1";	break;	//iLBC 8 kHz
	case 10:	ret = "speex/32000/1";	break;	//Speex 32 kHz
	case 11:	ret = "speex/16000/1";	break;	//Speex 16 kHz
	case 12:	ret = "speex/8000/1";	break;	//Speex 8 kHz
	case 13:	ret = "SILK/24000/1";	break;	//SILK 24 kHz
	case 14:	ret = "SILK/16000/1";	break;	//SILK 16 kHz
	case 15:	ret = "SILK/12000/1";	break;	//SILK 12 kHz
	case 16:	ret = "SILK/8000/1";	break;	//SILK 8 kHz
	case 17:	ret = "L16/44100/1";	break;	//LPCM 44 kHz
	case 18:	ret = "L16/44100/2";	break;	//LPCM 44 kHz Stereo
	case 19:	ret = "L16/16000/1";	break;	//LPCM 16 kHz
	case 20:	ret = "L16/16000/2";	break;	//LPCM 16 kHz Stereo
	case 21:	ret = "L16/8000/1";		break;	//LPCM 8 kHz
	case 22:	ret = "L16/8000/2";		break;	//LPCM 8 kHz Stereo
	default:
		break;
	}
	return ret;
}

CAccount::CAccount(CPjSipSDKBase * plugin):m_Plugin(plugin)
{
	this->log = log4cplus::Logger::getInstance("CAccount");
}

CAccount::~CAccount()
{
	this->m_callsmtx.lock();
	for (auto & call : m_calls)
	{
		delete call.second;
	}
	m_calls.clear();
	this->m_callsmtx.unlock();
}

void CAccount::onRegStarted(pj::OnRegStartedParam & prm)
{
	m_Plugin->onRegStarted(prm);
}

void CAccount::onRegState(pj::OnRegStateParam & prm)
{
	m_Plugin->onRegState(prm);
}

void CAccount::onCallState(const pj::CallInfo & ci)
{
	m_Plugin->onCallState(ci);
}

void CAccount::onDtmfDigit(pjsua_call_id call_id, const std::string & dtmf)
{
	m_Plugin->onDtmfDigit(call_id, dtmf);
}

void CAccount::onIncomingCall(pj::OnIncomingCallParam & prm)
{
	pj::Call *call = new CPCall(this, prm.callId);

	this->m_callsmtx.lock();
	for (auto & precall : this->m_calls) {
		precall.second->hangup(true);
	}
	this->m_calls[prm.callId] = call;
	this->m_callsmtx.unlock();

	pj::CallOpParam cprm(true);
	cprm.statusCode = PJSIP_SC_RINGING;
	call->answer(cprm);

	m_Plugin->onIncomingCall(call);
}

void CAccount::makeCall(const std::string & strCalled, pj::Call ** pcall)
{
	pj::Call * call = new CPCall(this);
	*pcall = call;
	pj::CallOpParam prm(true);
	prm.opt.audioCount = 1;
	call->makeCall(strCalled, prm);
	this->m_callsmtx.lock();
	this->m_calls[call->getId()] = call;
	this->m_callsmtx.unlock();
	return;
}
