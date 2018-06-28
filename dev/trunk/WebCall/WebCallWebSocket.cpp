#include "stdafx.h"
#include "WebCallWebSocket.h"
#include <log4cplus/loggingmacros.h>
//#include "CCPClient.h"
//#include "CCPClient_Internal.h"
#include <json/json.h>
#include "stringHelper.h"
#include "codingHelper.h"


std::list<WebCallWSclient *> g_WSClientSet;
std::recursive_mutex g_WSClientMtx;

WebCallWSclient::WebCallWSclient(struct lws * wsi)
	:WebSocketClient(wsi)
{
		this->log = log4cplus::Logger::getInstance("WebCallWSclient");
		//LOG4CPLUS_TRACE(log, m_SessionId << "construction");
};

WebCallWSclient::~WebCallWSclient()
{
	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	g_WSClientSet.remove(this);
	//LOG4CPLUS_TRACE(log, m_SessionId << "deconstruct");
}

void WebCallWSclient::OnOpen()
{
	//LOG4CPLUS_DEBUG(log, m_SessionId << "OnOpen");
	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	g_WSClientSet.push_back(this);
}

void WebCallWSclient::OnClose(const std::string & errorCode)
{
	//LOG4CPLUS_DEBUG(log, m_SessionId << "OnClose");
	delete this;
}

void WebCallWSclient::OnError(const std::string & errorCode)
{
	//LOG4CPLUS_DEBUG(log, m_SessionId << "OnError");
	//delete this;
}

void WebCallWSclient::OnSend()
{
}

void WebCallWSclient::OnMessage(const std::string & message)
{
	Json::Value jsonEvent;
	Json::Reader jsonReader;
	LOG4CPLUS_DEBUG(log, m_SessionId << "receive cmd" << message);

	if (jsonReader.parse(message, jsonEvent)) {

		if (jsonEvent.isObject() && jsonEvent["type"].isString() && jsonEvent["type"].asString() == "cmd") {

		}
		else {
			LOG4CPLUS_ERROR(log, m_SessionId << ", " << message << " not cmd type");
			return;
		}

		std::string cmd;
		if (jsonEvent["cmd"].isString()) {
			cmd = jsonEvent["cmd"].asString();
		}


		/********************deal request*********************/
		Json::Value cmdresult;
		cmdresult["type"] = "cmdresult";
		cmdresult["cmdresult"] = cmd;

		if (cmd == "initialize") {
			cmdresult["param"]["return"] = 0;//initialize();
		}
		else if (cmd == "unInitialize ") {
			cmdresult["param"]["return"] = 0;// unInitialize();
		}
		else if (cmd == "connectToCCPServer") {

			std::string serverAddr;
			uint32_t port = 0;
			std::string voipId;
			std::string voipPwd;
			if (jsonEvent["param"]["serverAddr"].isString()) {
				serverAddr = jsonEvent["param"]["serverAddr"].asString();
			}

			if (jsonEvent["param"]["serverPort"].isInt()) {
				port = jsonEvent["param"]["serverPort"].asInt();
			}

			if (jsonEvent["param"]["voipId"].isString()) {
				voipId = jsonEvent["param"]["voipId"].asString();
			}

			if (jsonEvent["param"]["voipPwd"].isString()) {
				voipPwd = jsonEvent["param"]["voipPwd"].asString();
			}

			//cmdresult["param"]["return"] = connectToCCP(serverAddr.c_str(), port, voipId.c_str(), voipPwd.c_str(), nullptr);
			cmdresult["param"]["return"] = connectToCCPServer(serverAddr.c_str(), port, voipId.c_str(), voipPwd.c_str());

		}
		else if (cmd == "disConnectToCCP") {
			cmdresult["param"]["return"] = disConnectToCCP();
		}
		else if (cmd == "getCurrentCall") {
			cmdresult["param"]["callid"] = getCurrentCall() != nullptr ? getCurrentCall() : "";
		}
		else if (cmd == "setTraceFlag") {
			bool enable = true;
			const char * fileName = "";
			uint32_t logLevel = 23;

			if (jsonEvent["param"]["enable"].isInt()) {
				enable = jsonEvent["param"]["enable"].isInt();
			}

			if (jsonEvent["param"]["logFileName"].isString()) {
				fileName = jsonEvent["param"]["logFileName"].asCString();
			}

			if (jsonEvent["param"]["logLevel"].isInt()) {
				logLevel = jsonEvent["param"]["logLevel"].asInt();
			}

			setTraceFlag(enable, fileName, logLevel);
			SetRegKey("LogLevel", logLevel);

			cmdresult["param"]["return"] = 0;

		}
		else if (cmd == "makeCall") {
			std::string caller;
			std::string called;

			if (jsonEvent["param"]["caller"].isString()) {
				caller = jsonEvent["param"]["caller"].asString();
			}

			if (jsonEvent["param"]["called"].isString()) {
				called = jsonEvent["param"]["called"].asString();
			}

			int ret = CWinSDKBase::makeCall(called.c_str());
			
			cmdresult["param"]["return"] = ret;
			if (getCurrentCall())
				cmdresult["param"]["callid"] = getCurrentCall();

		}
		else if (cmd == "acceptCall") {
			std::string callid;
			if (jsonEvent["param"]["callid"].isString()) {
				callid = jsonEvent["param"]["callid"].asString();
			}

			cmdresult["param"]["return"] = acceptCall(callid.c_str());
			cmdresult["param"]["callid"] = callid;

		}
		else if (cmd == "pauseCall") {

			std::string callid;

			if (jsonEvent["param"]["callid"].isString()) {
				callid = jsonEvent["param"]["callid"].asString();
			}

			//cmdresult["param"]["return"] = pauseCall(callid.c_str());
			//cmdresult["param"]["callid"] = callid;
			cmdresult["param"]["return"] = pauseCall(callid.c_str());
			cmdresult["param"]["callid"] = callid;

		}
		else if (cmd == "resumeCall") {

			std::string callid;

			if (jsonEvent["param"]["callid"].isString()) {
				callid = jsonEvent["param"]["callid"].asString();
			}

			//cmdresult["param"]["return"] = resumeCall(callid.c_str());
			//cmdresult["param"]["callid"] = callid;
			cmdresult["param"]["return"] = resumeCall(callid.c_str());
			cmdresult["param"]["callid"] = callid;
		}
		else if (cmd == "releaseCall") {

			std::string callid;

			uint32_t reason = 0;

			if (jsonEvent["param"]["callid"].isString()) {
				callid = jsonEvent["param"]["callid"].asString();
			}

			if (jsonEvent["param"]["reason"].isInt()) {
				reason = jsonEvent["param"]["reason"].asInt();
			}

			//cmdresult["param"]["return"] = releaseCall(callid.c_str(), reason);
			//cmdresult["param"]["callid"] = callid;

			cmdresult["param"]["return"] = releaseCall(callid.c_str(), reason);
			cmdresult["param"]["callid"] = callid;

		}
		else if (cmd == "sendDTMF") {

			std::string callid;
			std::string dtmf;

			if (jsonEvent["param"]["callid"].isString()) {
				callid = jsonEvent["param"]["callid"].asString();
			}

			if (jsonEvent["param"]["dtmf"].isString()) {
				dtmf = jsonEvent["param"]["dtmf"].asString();
			}

			for (auto & it : dtmf) {
				//cmdresult["param"]["return"] = sendDTMF(callid.c_str(), it);
				cmdresult["param"]["return"] = sendDTMF(callid.c_str(), it);
			}

			cmdresult["param"]["callid"] = callid;
			cmdresult["param"]["dtmf"] = dtmf;

		}
		else if (cmd == "setMute") {

			bool on = false;

			if (jsonEvent["param"]["on"].isInt()) {
				on = jsonEvent["param"]["on"].asInt();
			}

			cmdresult["param"]["return"] = setMute(on);

		}
		else if (cmd == "transferCall")
		{
			std::string callid;
			std::string dest;
			uint32_t type = 0;

			if (jsonEvent["param"]["callid"].isString()) {
				callid = jsonEvent["param"]["callid"].asString();
			}

			if (jsonEvent["param"]["dest"].isString()) {
				dest = jsonEvent["param"]["dest"].asString();
			}

			if (jsonEvent["param"]["type"].isInt()) {
				type = jsonEvent["param"]["type"].asInt();
			}

			cmdresult["param"]["return"] = transferCall(callid.c_str(), dest, type);
			cmdresult["param"]["callid"] = callid;
	
		}
		else if (cmd == "setCodecEnabled") {

			uint32_t type = 0;
			uint32_t enable = 0;

			if (jsonEvent["param"]["type"].isInt()) {
				type = jsonEvent["param"]["type"].asInt();
			}

			if (jsonEvent["param"]["enable"].isInt()) {
				enable = jsonEvent["param"]["enable"].asInt();
			}

			//cmdresult["param"]["return"] = setCodecEnabled(type, enable == 0 ? false : true);
			cmdresult["param"]["return"] = setCodecEnabled(type, enable == 0 ? false : true);
			cmdresult["param"]["type"] = type;
			cmdresult["param"]["enable"] = enable;

		}
		else if (cmd == "getCodecEnabled") {

			uint32_t type = 0;

			if (jsonEvent["param"]["type"].isInt()) {
				type = jsonEvent["param"]["type"].asInt();
			}

			cmdresult["param"]["return"] = getCodecEnabled(type);
			cmdresult["param"]["type"] = type;
			//cmdresult["param"]["enable"] = getCodecEnabled(type);

		}
		else if (cmd =="getMicroPhoneInfo")
		{
			cmdresult["param"]["return"] = getMicroPhoneInfo();
		}
		else if (cmd == "selectMicroPhone")
		{
			uint32_t index = 0;
			if (jsonEvent["param"]["index"].isInt()) {
				index = jsonEvent["param"]["index"].asInt();
			}
			cmdresult["param"]["return"] = selectMicroPhone(index);
		}
		else if (cmd == "setAudioRecordStatus") {
			std::string path;
			uint32_t status;

			if (jsonEvent["param"]["path"].isString()) {
				path = jsonEvent["param"]["path"].asString();
			}

			if (jsonEvent["param"]["enable"].isInt()) {
				status = jsonEvent["param"]["enable"].asInt();
			}

			//setAudioRecordStatus(path.c_str(), status == 1);
			cmdresult["param"]["return"] = 0;
		}
		else if (cmd == "getAudioRecordDeviceList") {
			/*MicroPhoneInfo* microphoneinfo = NULL;
			int microphoneCount = getMicroPhoneInfo(&microphoneinfo);
			cmdresult["param"]["return"] = 0;
			for (int i = 0; i < microphoneCount; i++) {
			cmdresult["param"]["microphone"][i]["name"] = microphoneinfo[i].name;
			cmdresult["param"]["microphone"][i]["index"] = microphoneinfo[i].index;
			cmdresult["param"]["microphone"][i]["guid"] = microphoneinfo[i].guid;
			}*/

		}
		else if (cmd == "getAudioPlayoutDeviceList") {
			/*SpeakerInfo *speakerinfo = NULL;
			int speakerCount = getSpeakerInfo(&speakerinfo);
			cmdresult["param"]["return"] = 0;
			for (int i = 0; i < speakerCount; i++) {
			cmdresult["param"]["speaker"][i]["name"] = speakerinfo[i].name;
			cmdresult["param"]["speaker"][i]["index"] = speakerinfo[i].index;
			cmdresult["param"]["speaker"][i]["guid"] = speakerinfo[i].guid;
			}*/
		}
		else if (cmd == "selectAudioRecordDevice") {
			uint32_t index;

			if (jsonEvent["param"]["index"].isInt()) {
				index = jsonEvent["param"]["index"].asInt();
			}

			//cmdresult["param"]["return"] = selectMicroPhone(index);
			SetRegKey("MicroPhone", index);
		}
		else if (cmd == "selectAudioPlayoutDevice") {
			uint32_t index;

			if (jsonEvent["param"]["index"].isInt()) {
				index = jsonEvent["param"]["index"].asInt();
			}

			//cmdresult["param"]["return"] = selectSpeaker(index);
			SetRegKey("Speaker", index);
		}
		else if (cmd == "setAudioPlayoutVolume")
		{
			uint32_t volume = 0;

			if (jsonEvent["param"]["volume"].isInt()) {
				volume = jsonEvent["param"]["volume"].asInt();
			}

			//cmdresult["param"]["return"] = setSpeakerVolume(volume);
			SetRegKey("SpeakerVolume", volume);
		}
		else if (cmd == "startMicrophoneTest") {
			//cmdresult["param"]["return"] = startMicrophoneTest();
		}
		else if (cmd == "stopMicrophoneTest") {
			//cmdresult["param"]["return"] = stopMicrophoneTest();
		}
		else if (cmd == "startPlayoutTest") {
			//cmdresult["param"]["return"] = startPlayoutTest();
		}
		else if (cmd == "stopPlayoutTest") {
			//cmdresult["param"]["return"] = stopPlayoutTest();
		}
		else if (cmd == "enableGlobalAudioInDevice") {
			uint32_t enable = 0;

			if (jsonEvent["param"]["enable"].isInt()) {
				enable = jsonEvent["param"]["enable"].asInt();
			}

			//cmdresult["param"]["return"] = enableGlobalAudioInDevice(enable == 1);
		}
		else if (cmd == "getVersion")
		{
			cmdresult["param"]["return"] = getVersion();
		}
		else if (cmd == "version")
		{
			cmdresult["param"]["return"] = getVersion();
		}
		else if (cmd == "setRegConfig")
		{
			cmdresult["param"]["return"] = 0;
			for (auto & it : jsonEvent["param"].getMemberNames()) {
				if (jsonEvent["param"][it].isInt()){
					SetRegKey(it.c_str(), jsonEvent["param"][it].asInt());
					if (it == "WebSocketPort"){
						//theApp.m_wsPort = jsonEvent["param"][it].asInt();
						std::thread th = std::thread([]() {
						//theApp.WSStop();
						//theApp.WSStart();
						});
						th.detach();
					}
				}
				else if (jsonEvent["param"][it].isString())
				{
					SetRegKey(it.c_str(), jsonEvent["param"][it].asString().c_str());
					if (it == "LogPath") {
					//	theApp.strLogPathDir = jsonEvent["param"][it].asString();
					//	theApp.LogConfig();
					}
				}
				
			}
		}

		Json::FastWriter writer;
		std::string result = writer.write(cmdresult);
		this->Send(result.c_str(), result.length());

	}
	else {
		//LOG4CPLUS_ERROR(log, m_SessionId << " OnMessage not json string:" << message);
	}

}

HWND WebCallWSclient::gethWnd() const
{
	return nullptr;
}

void WebCallWSclient::onConnect(unsigned int tcpMsgIdOut, int reason, const char *jsonString, int autoReconnect)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onConnect";

	out["param"]["tcpMsgIdOut"] = tcpMsgIdOut;
	out["param"]["reason"] = reason;
	out["param"]["jsonString"] = jsonString;
	out["param"]["autoReconnect"] = autoReconnect;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void WebCallWSclient::onIncomingCallReceived(int callType, int confType, const char *callid, const char *caller)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onIncomingCallReceived";

	out["param"]["caller"] = caller;
	out["param"]["callid"] = callid;
	out["param"]["callType"] = callType;
	out["param"]["confType"] = confType;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");

}

void WebCallWSclient::onCallProceeding(const char*callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onCallProceeding";

	out["param"]["callid"] = callid;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onCallAlerting(const char *callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onCallAlerting";

	out["param"]["callid"] = callid;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onCallAnswered(const char *callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onCallAnswered";

	out["param"]["callid"] = callid;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onCallReleased(const char * callid, int reason, int state, int CallEvent)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onCallReleased";

	out["param"]["callid"] = callid;
	out["param"]["reason"] = reason;
	out["param"]["state"] = state;
	out["param"]["CallEvent"] = CallEvent;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}


void WebCallWSclient::onCallPaused(const char* callid, int type, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onCallPaused";

	out["param"]["callid"] = callid;
	out["param"]["type"] = type;
	out["param"]["reason"] = reason;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onCallResumed(const char * callid, int type, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onCallResumed";

	out["param"]["callid"] = callid;
	out["param"]["type"] = type;
	out["param"]["reason"] = reason;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onSwitchCallMediaTypeRequest(const char * callid, int video, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onSwitchCallMediaTypeRequest";

	out["param"]["callid"] = callid;
	out["param"]["video"] = video;
	out["param"]["reason"] = reason;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onSwitchCallMediaTypeResponse(const char * callid, int video, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onSwitchCallMediaTypeResponse";

	out["param"]["callid"] = callid;
	out["param"]["video"] = video;
	out["param"]["reason"] = reason;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onRemoteVideoRatio(const char * CallidOrConferenceId, int width, int height, int type, const char * member, const char * ip, int port)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onRemoteVideoRatio";

	out["param"]["callid"] = CallidOrConferenceId;
	out["param"]["width"] = width;
	out["param"]["height"] = height;
	out["param"]["type"] = type;
	out["param"]["member"] = member;
	out["param"]["ip"] = ip;
	out["param"]["port"] = port;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onAudioData(const char * callid, const void * inData, int inLen, void * outData, int * outLen, bool send)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onAudioData";

	out["param"]["callid"] = callid;
	out["param"]["inData"] = std::string((const char *)inData,inLen);
	out["param"]["send"] = send;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onVideoData(const char * callid, const void * inData, int inLen, void * outData, int * outLen, bool send)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onVideoData";

	out["param"]["callid"] = callid;
	out["param"]["inData"] = std::string((const char *)inData, inLen);
	out["param"]["send"] = send;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onAudioCaptureData(const char * callid, unsigned char * data, int length, int samples, int sampleRate, int numChannels)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onAudioCaptureData";

	out["param"]["callid"] = callid;
	out["param"]["data"] = std::string((const char *)data, length);
	out["param"]["samples"] = samples;
	out["param"]["sampleRate"] = sampleRate;
	out["param"]["numChannels"] = numChannels;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onVideoCaptureData(const char * callid, unsigned char * data, int length, int width, int height, int y_stride, int uv_stride)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onVideoCaptureData";

	out["param"]["callid"] = callid;
	out["param"]["data"] = std::string((const char *)data, length);
	out["param"]["width"] = width;
	out["param"]["height"] = height;
	out["param"]["y_stride"] = y_stride;
	out["param"]["uv_stride"] = uv_stride;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onMediaDestinationChanged(const char * callid, int mediaType, const char * ip, int port, int type)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onMediaDestinationChanged";

	out["param"]["callid"] = callid;
	out["param"]["mediaType"] = mediaType;
	out["param"]["ip"] = ip;
	out["param"]["port"] = port;
	out["param"]["type"] = type;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onNoCamera(const char * callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onNoCamera";

	out["param"]["callid"] = callid;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onNoMicRecording(const char * callid, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onNoMicRecording";

	out["param"]["callid"] = callid;
	out["param"]["reason"] = reason;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onCallTransfered(const char * callid, const char * destionation, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onCallTransfered";

	out["param"]["callid"] = callid;
	out["param"]["destionation"] = destionation;
	out["param"]["reason"] = reason;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onMeetingTransfered(const char * callid, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onMeetingTransfered";

	out["param"]["callid"] = callid;
	out["param"]["reason"] = reason;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onAudioEnergyFeedback(int averageEnergy)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onAudioEnergyFeedback";

	out["param"]["averageEnergy"] = averageEnergy;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onSipConnect(int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onSipConnect";

	out["param"]["reason"] = reason;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onSipLogOut(int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onSipLogOut";

	out["param"]["reason"] = reason;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}


void WebCallWSclient::onDtmfReceived(const char *callid, char dtmf)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onDtmfReceived";

	out["param"]["callid"] = callid;

	std::stringstream stream;
	stream << dtmf;

	out["param"]["dtmf"] = stream.str();

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onLogInfo(const char* loginfo)
{
	//LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onLogInfo";

	out["param"]["loginfo"] = loginfo;

	std::string strout;
	strout = writer.write(out);

	std::unique_lock<std::recursive_mutex> lck(g_WSClientMtx);
	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	//LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onLogOut(unsigned int tcpMsgIdOut, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onLogOut";

	out["param"]["tcpMsgIdOut"] = tcpMsgIdOut;
	out["param"]["reason"] = reason;

	std::string strout;
	strout = writer.write(out);

	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onWillCloseTcp(void)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "onWillCloseTcp";

	std::string strout;
	strout = writer.write(out);

	for (auto & it : g_WSClientSet) {
		it->Send(strout.c_str(), strout.length());
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}


WebCallWSServer::WebCallWSServer(int port)
	:WebSocketServer(port)
{
}
WebSocket::WebSocketClient * WebCallWSServer::OnAccept(struct lws *wsi)
{
	WebSocket::WebSocketClient * wsc = new WebCallWSclient(wsi);
	return wsc;
}
