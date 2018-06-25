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


		if (!(jsonEvent["type"].isString() && jsonEvent["type"].asString() == "cmd"))
		{
			//LOG4CPLUS_ERROR(log, m_SessionId << ", " << message << " not cmd type");
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

		if (cmd == "init") {
			cmdresult["param"]["return"] = 0;
		}
		else if (cmd == "unInit") {
			cmdresult["param"]["return"] = 0;
		}
		else if (cmd == "login") {

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
			cmdresult["param"]["return"] = connectToCCP(serverAddr.c_str(), port, voipId.c_str(), voipPwd.c_str());

		}
		else if (cmd == "logout") {
			cmdresult["param"]["return"] = disConnectToCCP();
		}
		else if (cmd == "setLogLevel") {
			const char * fileName = nullptr;
			uint32_t logLevel = 23;

			if (jsonEvent["param"]["fileName"].isString()) {
				fileName = jsonEvent["param"]["fileName"].asCString();
			}

			if (jsonEvent["param"]["logLevel"].isInt()) {
				logLevel = jsonEvent["param"]["logLevel"].asInt();
			}

			//setTraceFlag(fileName, logLevel);
			SetRegKey("LogLevel", logLevel);

			cmdresult["param"]["return"] = 0;

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
		else if (cmd == "makeCall") {
			std::string caller;
			std::string called;

			if (jsonEvent["param"]["caller"].isString()) {
				caller = jsonEvent["param"]["caller"].asString();
			}

			if (jsonEvent["param"]["called"].isString()) {
				called = jsonEvent["param"]["called"].asString();
			}


			/* char *callid = makeCall(VOICE_CALL, called.c_str());
			if (!callid) {
				cmdresult["param"]["return"] = -1;
				cmdresult["param"]["callid"] = "";
			}
			else {
				cmdresult["param"]["return"] = 0;
				cmdresult["param"]["callid"] = callid;
			}*/

			std::string strCallId = CPjSipSDK::makeCall(called.c_str());
			if (strCallId.empty()) {
				cmdresult["param"]["return"] = -1;
				cmdresult["param"]["callid"] = strCallId;
			}
			else {
				cmdresult["param"]["return"] = 0;
				cmdresult["param"]["callid"] = strCallId;
			}

		}
		else if (cmd == "answerCall") {
			std::string callid;
			int iCallid;

			if (jsonEvent["param"]["callid"].isString()) {
				callid = jsonEvent["param"]["callid"].asString();
				iCallid = callid == "" ? 0 : std::stoi(callid.c_str());
			}else if (jsonEvent["param"]["callid"].isInt()) {
				iCallid = jsonEvent["param"]["callid"].asInt();
			}


			//cmdresult["param"]["return"] = acceptCallByMediaType(callid.c_str(), 0);
			//cmdresult["param"]["callid"] = callid;
			//int iCallid = callid == "" ? 0 : std::stoi(callid.c_str());
			LOG4CPLUS_DEBUG(log, "callid:" << iCallid);
			cmdresult["param"]["return"] = CPjSipSDK::acceptCall(iCallid);
			cmdresult["param"]["callid"] = callid;

		}
		else if (cmd == "rejectCall") {

			std::string callid;
			uint32_t reason = 0;

			if (jsonEvent["param"]["callid"].isString()) {
				callid = jsonEvent["param"]["callid"].asString();
			}

			if (jsonEvent["param"]["reason"].isInt()) {
				reason = jsonEvent["param"]["reason"].asInt();
			}

			//cmdresult["param"]["return"] = rejectCall(callid.c_str(), reason);
			//cmdresult["param"]["callid"] = callid;
			int iCallid = callid == "" ? 0 : std::stoi(callid.c_str());
			cmdresult["param"]["return"] = CPjSipSDK::rejectCall(iCallid, reason);
			cmdresult["param"]["callid"] = callid;

		}
		else if (cmd == "pauseCall") {

			std::string callid;

			if (jsonEvent["param"]["callid"].isString()) {
				callid = jsonEvent["param"]["callid"].asString();
			}

			//cmdresult["param"]["return"] = pauseCall(callid.c_str());
			//cmdresult["param"]["callid"] = callid;
			int iCallid = callid == "" ? 0 : std::stoi(callid.c_str());
			cmdresult["param"]["return"] = CPjSipSDK::pauseCall(iCallid);
			cmdresult["param"]["callid"] = callid;

		}
		else if (cmd == "resumeCall") {

			std::string callid;

			if (jsonEvent["param"]["callid"].isString()) {
				callid = jsonEvent["param"]["callid"].asString();
			}

			//cmdresult["param"]["return"] = resumeCall(callid.c_str());
			//cmdresult["param"]["callid"] = callid;
			int iCallid = callid == "" ? 0 : std::stoi(callid.c_str());
			cmdresult["param"]["return"] = CPjSipSDK::resumeCall(iCallid);
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
			int iCallid = callid == "" ? -1 : std::stoi(callid.c_str());
			cmdresult["param"]["return"] = CPjSipSDK::releaseCall(iCallid);
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
				int iCallid = callid == "" ? 0 : std::stoi(callid.c_str());
				cmdresult["param"]["return"] = CPjSipSDK::sendDTMF(iCallid, it);
			}

			cmdresult["param"]["callid"] = callid;
			cmdresult["param"]["dtmf"] = dtmf;

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
			cmdresult["param"]["return"] = CPjSipSDK::setCodecEnabled(type, enable == 0 ? false : true);
			cmdresult["param"]["type"] = type;
			cmdresult["param"]["enable"] = enable;

		}
		else if (cmd == "getCodecEnabled") {

			uint32_t type = 0;

			if (jsonEvent["param"]["type"].isInt()) {
				type = jsonEvent["param"]["type"].asInt();
			}

			cmdresult["param"]["return"] = 0;
			cmdresult["param"]["type"] = type;
			//cmdresult["param"]["enable"] = getCodecEnabled(type);
			cmdresult["param"]["enable"] = CPjSipSDK::getCodecEnabled(type);

		}
		else if (cmd == "getVersion")
		{
			cmdresult["param"]["return"] = 0;
			cmdresult["param"]["version"] = getVersion();
		}
		else if (cmd == "version")
		{
			cmdresult["param"]["return"] = 0;
			cmdresult["param"]["version"] = getVersion();
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
		else if (helper::string::toLower(cmd) == "mute") {
			//cmdresult["param"]["return"] = setMute(true);
			cmdresult["param"]["return"] = CPjSipSDK::setMute(true);
		}
		else if (helper::string::toLower(cmd) == "unmute") {
			//cmdresult["param"]["return"] = setMute(false);
			cmdresult["param"]["return"] = CPjSipSDK::setMute(false);
		}
		else if (cmd == "getCurrentCall") {
			cmdresult["param"]["callid"] = getCurrentCall()>=0 ? std::to_string(getCurrentCall()) : "";
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

void WebCallWSclient::onGetCapabilityToken()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	//PostMessage(gethWnd(), WM_onGetCapabilityToken, NULL, NULL);
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void WebCallWSclient::onConnected()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnConnected";

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void WebCallWSclient::onConnectError(int reason, const char * desc)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnConnectError";
	out["param"]["reason"] = reason;
	out["param"]["desc"] = ASCII2UTF_8("Á¬½ÓÊ§°Ü");

	std::string strout;
	strout = writer.write(out);
	this->Send(strout.c_str(), strout.length());
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");

}

void WebCallWSclient::onIncomingCallReceived(int callType, const char *callid, const char *caller)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnIncomingCallReceived";

	out["param"]["caller"] = caller;
	out["param"]["callid"] = callid;
	out["param"]["called"] = "";

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");

}

void WebCallWSclient::onCallProceeding(const char*callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnCallProceeding";

	out["param"]["caller"] = "";
	out["param"]["callid"] = callid;
	out["param"]["called"] = "";

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onCallAlerting(const char *callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnCallAlerting";

	out["param"]["caller"] = "";
	out["param"]["callid"] = callid;
	out["param"]["called"] = "";

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onCallAnswered(const char *callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnCallAnswered";

	out["param"]["caller"] = "";
	out["param"]["callid"] = callid;
	out["param"]["called"] = "";

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onMakeCallFailed(const char *callid, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnMakeCallFailed";

	out["param"]["caller"] = "";
	out["param"]["callid"] = callid;
	out["param"]["called"] = "";
	out["param"]["reason"] = reason;

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onCallPaused(const char* callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnCallPaused";

	out["param"]["caller"] = "";
	out["param"]["callid"] = callid;
	out["param"]["called"] = "";

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onCallPausedByRemote(const char *callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnCallPausedByRemote";

	out["param"]["caller"] = "";
	out["param"]["callid"] = callid;
	out["param"]["called"] = "";

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onCallReleased(const char *callid, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnCallReleased";

	out["param"]["caller"] = "";
	out["param"]["callid"] = callid;
	out["param"]["called"] = "";
	out["param"]["reason"] = 0;

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onCallTransfered(const char *callid, const char *destionation)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");

	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnCallTransfered";

	out["param"]["caller"] = "";
	out["param"]["callid"] = callid;
	out["param"]["destionation"] = (destionation ? destionation : "");

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onDtmfReceived(const char *callid, char dtmf)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnDtmfReceived";

	out["param"]["caller"] = "";
	out["param"]["callid"] = callid;
	out["param"]["called"] = "";

	std::stringstream stream;
	stream << dtmf;

	out["param"]["dtmf"] = stream.str();

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onTextMessageReceived(const char *sender, const char *receiver, const char *sendtime, const char *msgid, const char *message, const char *userdata)
{

}
void WebCallWSclient::onMessageSendReport(const char*msgid, const char*time, int status)
{

}
void WebCallWSclient::onLogInfo(const char* loginfo)
{

	//Json::Value root;
	//root["loginfo"] = (loginfo ? loginfo : "");
	//std::string * param = new std::string();
	//*param = root.toStyledString();
	//::PostMessage(gethWnd(), WM_onLogInfo, NULL, reinterpret_cast<LPARAM>(param));
}
void WebCallWSclient::onResumed(const char* callid)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnResumed";

	out["param"]["caller"] = "";
	out["param"]["callid"] = callid;
	out["param"]["called"] = "";

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void WebCallWSclient::onNotifyGeneralEvent(const char*callid, int eventType, const char*userdata, int intdata)
{

}
void WebCallWSclient::onCallMediaUpdateRequest(const char*callid, int request)
{

}
void WebCallWSclient::onCallMediaUpdateResponse(const char*callid, int response)
{

}
void WebCallWSclient::onDeliverVideoFrame(const char*callid, unsigned char*buf, int size, int width, int height)
{

}
void WebCallWSclient::onRecordVoiceStatus(const char *callid, const char *fileName, int status)
{

}
void WebCallWSclient::onAudioData(const char *callid, const void *inData, int inLen, void *outData, int &outLen, bool send)
{

}
void WebCallWSclient::onOriginalAudioData(const char *callid, const void *inData, int inLen, int sampleRate, int numChannels, const char *codec, bool send)
{

}
void WebCallWSclient::onMessageRemoteVideoRotate(const char *degree)
{

}
void WebCallWSclient::onRequestSpecifiedVideoFailed(const char *callid, const char *sip, int reason)
{

}
void WebCallWSclient::onStopSpecifiedVideoResponse(const char *callid, const char *sip, int response, void *window)
{

}
void WebCallWSclient::onEnableSrtp(const char *sip, bool isCaller)
{

}
void WebCallWSclient::onRemoteVideoRatioChanged(const char *callid, int width, int height, bool isVideoConference, const char *sipNo)
{

}
void WebCallWSclient::onLogOut()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value out;
	Json::FastWriter writer;
	out["type"] = "event";
	out["event"] = "OnLogOut";

	std::string strout;
	strout = writer.write(out);

	this->Send(strout.c_str(), strout.length());
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}
void WebCallWSclient::oneXosipThreadStop()
{

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
