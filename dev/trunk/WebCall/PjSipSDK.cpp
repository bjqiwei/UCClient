#include "stdafx.h"
#include "PjSipSDK.h"
#include <log4cplus/loggingmacros.h>
#include <json/json.h>
#include "codingHelper.h"


CPjSipSDK::CPjSipSDK()
{
	std::string utf8RingFile = utf8Dir + "ring.wav";
	this->setRingFile(utf8RingFile);
}


CPjSipSDK::~CPjSipSDK()
{
}

void CPjSipSDK::onGetCapabilityToken()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	PostMessage(gethWnd(), WM_onGetCapabilityToken, NULL, NULL);
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void CPjSipSDK::onConnected()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	PostMessage(gethWnd(), WM_onConnected, NULL, NULL);
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void CPjSipSDK::onConnectError(int reason, const char * desc)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	Json::Value root;
	root["reason"] = reason;
	root["desc"] = desc;
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	PostMessage(gethWnd(), WM_onConnectError, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void CPjSipSDK::onIncomingCallReceived(int callType, const char *callid, const char *caller)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callType"] = callType;
	root["callid"] = callid ? callid : "";
	root["caller"] = caller ? caller : "";

	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onIncomingCallReceived, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");

}
void CPjSipSDK::onCallProceeding(const char*callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = callid ? callid : "";
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onCallProceeding, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onCallAlerting(const char *callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = callid ? callid : "";
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onCallAlerting, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onCallAnswered(const char *callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = callid ? callid : "";
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onCallAnswered, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onMakeCallFailed(const char *callid, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = callid ? callid : "";
	root["reason"] = reason;
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onMakeCallFailed, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onCallPaused(const char* callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = callid ? callid : "";
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onCallPaused, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onCallPausedByRemote(const char *callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = callid ? callid : "";
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onCallPausedByRemote, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onCallReleased(const char *callid, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = callid ? callid : "";
	root["reason"] = reason;
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onCallReleased, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onCallTransfered(const char *callid, const char *destionation)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = (callid ? callid : "");
	root["destionation"] = (destionation ? destionation : "");
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onCallTransfered, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onDtmfReceived(const char *callid, char dtmf)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = (callid ? callid : "");
	root["dtmf"] = dtmf;
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onDtmfReceived, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onTextMessageReceived(const char *sender, const char *receiver, const char *sendtime, const char *msgid, const char *message, const char *userdata)
{

}
void CPjSipSDK::onMessageSendReport(const char*msgid, const char*time, int status)
{

}
void CPjSipSDK::onLogInfo(const char* loginfo)
{
	
	//Json::Value root;
	//root["loginfo"] = (loginfo ? loginfo : "");
	//std::string * param = new std::string();
	//*param = root.toStyledString();
	//::PostMessage(gethWnd(), WM_onLogInfo, NULL, reinterpret_cast<LPARAM>(param));
}
void CPjSipSDK::onResumed(const char* callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = (callid ? callid : "");
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onResumed, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onNotifyGeneralEvent(const char*callid, int eventType, const char*userdata, int intdata)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = callid ? callid : "";
	root["eventType"] = eventType;
	root["userdata"] = userdata ? userdata : "";
	root["intdata"] = intdata;
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onNotifyGeneralEvent, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");

}
void CPjSipSDK::onCallMediaUpdateRequest(const char*callid, int request)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = callid ? callid : "";
	root["request"] = request;
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onCallMediaUpdateRequest, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onCallMediaUpdateResponse(const char*callid, int response)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["callid"] = callid ? callid : "";
	root["response"] = response;
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onCallMediaUpdateResponse, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onDeliverVideoFrame(const char*callid, unsigned char*buf, int size, int width, int height)
{

}
void CPjSipSDK::onRecordVoiceStatus(const char *callid, const char *fileName, int status)
{

}
void CPjSipSDK::onAudioData(const char *callid, const void *inData, int inLen, void *outData, int &outLen, bool send)
{

}
void CPjSipSDK::onOriginalAudioData(const char *callid, const void *inData, int inLen, int sampleRate, int numChannels, const char *codec, bool send)
{

}
void CPjSipSDK::onMessageRemoteVideoRotate(const char *degree)
{

}
void CPjSipSDK::onRequestSpecifiedVideoFailed(const char *callid, const char *sip, int reason)
{

}
void CPjSipSDK::onStopSpecifiedVideoResponse(const char *callid, const char *sip, int response, void *window)
{

}
void CPjSipSDK::onEnableSrtp(const char *sip, bool isCaller)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value root;
	root["sip"] = sip ? sip : "";
	root["isCaller"] = isCaller;
	std::string * param = new std::string();
	*param = root.toStyledString();
	LOG4CPLUS_TRACE(log, __FUNCTION__ << ":" << *param);
	::PostMessage(gethWnd(), WM_onEnableSrtp, NULL, reinterpret_cast<LPARAM>(param));
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::onRemoteVideoRatioChanged(const char *callid, int width, int height, bool isVideoConference, const char *sipNo)
{

}
void CPjSipSDK::onLogOut()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	::PostMessage(gethWnd(), WM_onLogOut, NULL, NULL);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void CPjSipSDK::oneXosipThreadStop()
{

}