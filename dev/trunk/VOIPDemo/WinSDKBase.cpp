#include "stdafx.h"
#include "WinSDKBase.h"
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loggingmacros.h>
#include <atomic>
#include <map>
#include <mutex>
#include "codingHelper.h"

#pragma comment(lib,"version.lib")

static std::atomic_ulong g_pjsipReferce = 0;
static CWinSDKBase * g_Instance = nullptr;
static CCallbackInterface CB;

void CWinSDKBase::sonLogInfo(const char* loginfo) // 用于接收底层的log信息,调试出现的问题.
{
	//if (g_Instance) g_Instance->onLogInfo(loginfo);
}

void CWinSDKBase::sonConnect(unsigned int tcpMsgIdOut, int reason, const char *jsonString, int autoReconnect)
{
	if (g_Instance) g_Instance->onConnect(tcpMsgIdOut, reason, jsonString, autoReconnect);
}

void CWinSDKBase::sonLogOut(unsigned int tcpMsgIdOut, int reason)
{
	if (g_Instance) g_Instance->onLogOut(tcpMsgIdOut, reason);
}

void CWinSDKBase::sonWillCloseTcp(void)
{
	if (g_Instance) g_Instance->onWillCloseTcp();
}

void CWinSDKBase::sonIncomingCallReceived(int callType, int confType, const char *callid, const char *caller)  //有呼叫呼入
{
	::alertingCall(callid);
	if (g_Instance) g_Instance->onIncomingCallReceived(callType, confType, callid, caller);
}

void CWinSDKBase::sonCallProceeding(const char*callied)		//呼叫已经被云通讯平台处理
{
	if (g_Instance) g_Instance->onCallProceeding(callied);
}
void CWinSDKBase::sonCallAlerting(const char *callid)		//呼叫振铃
{
	if (g_Instance) g_Instance->onCallAlerting(callid);
}
void CWinSDKBase::sonCallAnswered(const char *callid)			//外呼对方应答
{
	if (g_Instance) g_Instance->onCallAnswered(callid);
}

void CWinSDKBase::sonCallReleased(const char *callid, int reason, int state, int CallEvent)			//呼叫挂机
{
	if (g_Instance) g_Instance->onCallReleased(callid, reason, state, CallEvent);
}

void CWinSDKBase::sonDtmfReceived(const char *callid, char dtmf)		//收到DTMF按键时的回调
{
	if (g_Instance) g_Instance->onDtmfReceived(callid, dtmf);
}

void CWinSDKBase::sonCallPaused(const char* callid, int type, int reason)				//本地Pause呼叫成功
{
	if (g_Instance) g_Instance->onCallPaused(callid, type, reason);
}

void CWinSDKBase::sonCallResumed(const char* callid, int type, int reason)
{
	if (g_Instance) g_Instance->onCallResumed(callid, type, reason);
}

void CWinSDKBase::sonSwitchCallMediaTypeRequest(const char*callid, int video, int reason) 
{
	if (g_Instance) g_Instance->sonSwitchCallMediaTypeRequest(callid, video, reason);
}

void CWinSDKBase::sonSwitchCallMediaTypeResponse(const char*callid, int video, int reason)
{
	if (g_Instance) g_Instance->onSwitchCallMediaTypeResponse(callid, video, reason);
}

void CWinSDKBase::sonRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member, const char *ip, int port)
{
	if (g_Instance) g_Instance->onRemoteVideoRatio(CallidOrConferenceId, width, height, type, member, ip, port);
}

void CWinSDKBase::sonAudioData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send) //在音频数据发送之前，将音频数据返回给上层处理，然后将上层处理后的数据返回来。
{
	if (g_Instance) g_Instance->onAudioData(callid, inData, inLen, outData, outLen, send);
}

void CWinSDKBase::sonVideoData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send)
{
	if (g_Instance) g_Instance->onVideoData(callid, inData, inLen, outData, outLen, send);
}

void CWinSDKBase::sonAudioCaptureData(const char *callid, unsigned char* data, int length, int samples, int sampleRate, int numChannels)
{
	if (g_Instance) g_Instance->onAudioCaptureData(callid, data, length, samples, sampleRate, numChannels);
}

void CWinSDKBase::sonVideoCaptureData(const char *callid, unsigned char* data, int length, int width, int height, int y_stride, int uv_stride)
{
	if (g_Instance) g_Instance->onVideoCaptureData(callid, data, length, width, height, y_stride, uv_stride);
}

void CWinSDKBase::sonMediaDestinationChanged(const char* callid, int mediaType, const char *ip, int port, int type)
{
	if (g_Instance) g_Instance->onMediaDestinationChanged(callid, mediaType, ip, port, type);
}

void CWinSDKBase::sonNoCamera(const char *callid)
{
	if (g_Instance) g_Instance->onNoCamera(callid);
}

void CWinSDKBase::sonNoMicRecording(const char *callid, int reason)
{
	if (g_Instance) g_Instance->onNoMicRecording(callid, reason);
}

void CWinSDKBase::sonCallTransfered(const char *callid, const char *destionation, int reason)
{
	if (g_Instance) g_Instance->onCallTransfered(callid, destionation, reason);
}

void CWinSDKBase::sonMeetingTransfered(const char *callid, int reason)
{
	if (g_Instance) g_Instance->onMeetingTransfered(callid, reason);
}

void CWinSDKBase::sonAudioEnergyFeedback(int averageEnergy)
{
	if (g_Instance) g_Instance->onAudioEnergyFeedback(averageEnergy);
}

void CWinSDKBase::sonSipConnect(int reason)
{
	if (g_Instance) g_Instance->onSipConnect(reason);
}

void CWinSDKBase::sonSipLogOut(int reason)
{
	if (g_Instance) g_Instance->onSipLogOut(reason);
}

std::string CWinSDKBase::getVersion()
{
	static std::string result;
	if (!result.empty())
		return result;

	result = ::getVersion();
	return result;
}

CWinSDKBase::CWinSDKBase()
{
	log = log4cplus::Logger::getInstance("CPjSipSDKBase");
	LOG4CPLUS_INFO(log, "SDK Version:" << getVersion());
	LOG4CPLUS_INFO(log, "SDK:" << utf8ModuleFilename);
	LOG4CPLUS_TRACE(log, "construction");
	g_Instance = this;
}

CWinSDKBase::~CWinSDKBase()
{
	//pjsua_acc_set_user_data(m_acc->getId(), NULL);
	LOG4CPLUS_TRACE(log, "destruction");
	g_Instance = nullptr;
}

int CWinSDKBase::initialize()
{
	if (g_pjsipReferce.fetch_add(1) == 0) {

		//LOG4CPLUS_DEBUG(log, __FUNCTION__);
		CB.onLogInfo = sonLogInfo;
		CB.onConnect = sonConnect;
		CB.onLogOut = sonLogOut;
		CB.onWillCloseTcp = sonWillCloseTcp;
		CB.onIncomingCallReceived = sonIncomingCallReceived;
		CB.onCallProceeding = sonCallProceeding;
		CB.onCallAlerting = sonCallAlerting;
		CB.onCallAnswered = sonCallAnswered;
		CB.onCallReleased = sonCallReleased;
		CB.onDtmfReceived = sonDtmfReceived;
		CB.onCallPaused = sonCallPaused;
		CB.onCallResumed = sonCallResumed;
		CB.onSwitchCallMediaTypeRequest = sonSwitchCallMediaTypeRequest;
		CB.onSwitchCallMediaTypeResponse = sonSwitchCallMediaTypeResponse;
		CB.onRemoteVideoRatio = sonRemoteVideoRatio;
		CB.onAudioData = sonAudioData;
		CB.onVideoData = sonVideoData;
		CB.onAudioCaptureData = sonAudioCaptureData;
		CB.onVideoCaptureData = sonVideoCaptureData;
		CB.onMediaDestinationChanged = sonMediaDestinationChanged;
		CB.onNoCamera = sonNoCamera;
		CB.onNoMicRecording = sonNoMicRecording;
		CB.onCallTransfered = sonCallTransfered;
		CB.onMeetingTransfered = sonMeetingTransfered;
		CB.onAudioEnergyFeedback = sonAudioEnergyFeedback;
		CB.onSipConnect = sonSipConnect;
		CB.onSipLogOut = sonSipLogOut;

		//LOG4CPLUS_DEBUG(log, "SetCallBackFuction");

		int ret = ::servicecoreInitialize(&CB);
		::setCodecEnabled(codec_VP8, 0);
		::setCodecEnabled(codec_H264, 0);
		::setCodecEnabled(codec_H264_HIGH, 0);
		//LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
		return ret;

	}

	return 0;
}

void CWinSDKBase::setRing(const std::string & filename)
{ 
	LOG4CPLUS_DEBUG(log, __FUNCTION__ << ",filename:" << filename);
	int ret = 0;// ::setRing(filename.c_str());
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ;
}

int CWinSDKBase::connectToCCPServer(std::string utf8Addr, LONG port, std::string utf8voipId, std::string utf8voipPwd)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " addr:" << utf8Addr << ", port:" << port << ",voipId:" << utf8voipId << ", pwd:" << utf8voipPwd);

	this->m_addr = utf8Addr;
	this->port = port;
	this->m_voipid = utf8voipId;
	this->m_voippwd = utf8voipPwd;

	int ret = ::connectToCCPServer(this->m_addr.c_str(), this->port, this->m_voipid.c_str(), this->m_voippwd.c_str());

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return 0;
}

int CWinSDKBase::makeCall(std::string strCalled) {

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " called:" << strCalled);
	
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", callType:" << 0 << ",called:" << strCalled);
	const char * callid = nullptr;
	int ret = ::makeCall(&callid, 0 , strCalled.c_str());
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret );
	return ret;
}

int CWinSDKBase::acceptCall(const char *callid) {
	LOG4CPLUS_DEBUG(log, __FUNCTION__ << ",callid:" << (callid ? callid : "NULL"));
	int ret = ::acceptCall(callid, 0);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}


int CWinSDKBase::pauseCall(const char * callid)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ << ",callid:" << (callid ? callid : "NULL"));
	int ret = ::pauseCall(callid);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int CWinSDKBase::resumeCall(const char * callid)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ << ",callid:" << (callid ? callid : "NULL"));
	int ret = ::resumeCall(callid);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int CWinSDKBase::transferCall(const char * callid, std::string destination, int type)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ << ",callid:" << (callid ? callid : "NULL") << ",destination:"
		<< destination << ",type:" << type);
	int ret = ::transferCall(callid, destination.c_str(), type);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int CWinSDKBase::sendDTMF(const char * callid, const char dtmf)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ << ",callid:" << (callid ? callid : "NULL") << ",dtmf:" << dtmf);
	int ret = ::sendDTMF(callid, dtmf);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int CWinSDKBase::releaseCall(const char * callid, int reason) {

	LOG4CPLUS_DEBUG(log, __FUNCTION__ << ",callid:" << (callid ? callid : "NULL") << " reason:" << reason);
	int ret = ::releaseCall(callid, reason);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

const char * CWinSDKBase::getCurrentCall() const
{
	const char * ret = ::getCurrentCall();
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << (ret ? ret : "NULL"));
	return ret;
}

int CWinSDKBase::disConnectToCCP()
{
	int ret = ::disConnectToCCP(nullptr);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int CWinSDKBase::unInitialize()
{
	if (g_pjsipReferce.fetch_sub(1) == 1) {
		//LOG4CPLUS_DEBUG(log, __FUNCTION__);
		//g_Instance = nullptr;
		int ret = ::unInitialize();
		//LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
		return ret;
	}
	return 0;
}

void CWinSDKBase::setTraceFlag(bool enable, const char * logFileName, int level)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ "enable:" << enable << " logFileName:" << logFileName << " level:" << level);
	::setTraceFlag(enable, logFileName, level, nullptr, nullptr, -1, -1, 0, -1);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:");
}

int CWinSDKBase::setCodecEnabled(int codecid, int enabled)
{
	return ::setCodecEnabled(codecid, enabled);
}

bool CWinSDKBase::getCodecEnabled(int codecid)
{
	return ::getCodecEnabled(codecid);
}

int CWinSDKBase::setMute(bool on)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ "on:" << on);
	int ret = ::setMute(on);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

Json::Value CWinSDKBase::getMicroPhoneInfo()
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ );
	CCPMicroPhoneInfo * micro = nullptr;
	Json::Value info(Json::arrayValue);
	int count = ::getMicroPhoneInfo(&micro);
	for (int i = 0; i < count; i++) {
		Json::Value jmicro;
		jmicro["guid"] = micro[i].guid;
		jmicro["index"] = micro[i].index;
		jmicro["name"] = micro[i].name;
		info.append(jmicro);
	}
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << info.toStyledString());
	return info;
}

int CWinSDKBase::selectMicroPhone(int microphoneIndex)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__);
	int ret = ::selectMicroPhone(microphoneIndex);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int CWinSDKBase::setUserData(int type, const char *data)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " type:" << type << ", data:" << data);
	int ret = ::setUserData(type, data);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int CWinSDKBase::consultTransferCall(const char * callid, const char * consultCallid, const char * destination)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " callid:" << callid << ", consultCallid:" << consultCallid << ", destination:" << destination);
	int ret = ::consultTransferCall(callid, consultCallid, destination);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int CWinSDKBase::transferMeeting(int type, const char *callid, const char *consultCallid, const char *consultedUser)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ "type:" << type << ", callid:" << callid << ", consultCallid:" << consultCallid << ", consultedUser:" << consultedUser);
	int ret = ::transferMeeting(type, callid, consultCallid, consultedUser);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int CWinSDKBase::setSipTransportType(int transType)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ "transType:" << transType);
	int ret = ::setSipTransportType(transType);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

void CWinSDKBase::setSrtpEnabled(int TransportType, int cryptoType)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ "TransportType:" << TransportType << ", cryptoType:" << cryptoType);
	::setSrtpEnabled(TransportType, cryptoType);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:");
}
