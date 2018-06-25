#pragma once
#include "stdafx.h"
#include "PjSipSDKBase.h"

class CPjSipSDK :
	public CPjSipSDKBase
{
protected:
	enum {
		WM_onGetCapabilityToken = WM_USER + 111,
		WM_onCallAlerting,
		WM_onCallAnswered,
		WM_onCallPaused,
		WM_onCallProceeding,
		WM_onCallReleased,
		WM_onCallTransfered,
		WM_onConnected,
		WM_onConnectError,
		WM_onIncomingCallReceived,
		WM_onMakeCallFailed,
		WM_onCallPausedByRemote,
		WM_onDtmfReceived,
		WM_onLogInfo,
		WM_onResumed,
		WM_onNotifyGeneralEvent,
		WM_onCallMediaUpdateRequest,
		WM_onCallMediaUpdateResponse,
		WM_onEnableSrtp,
		WM_onLogOut,
	};
public:
	CPjSipSDK();
	virtual ~CPjSipSDK();

	virtual HWND  gethWnd() const = 0;

private:
	virtual void onGetCapabilityToken() override;			//由APP提供的获取能力token的回调方法
	virtual void onConnected() override;					//与云通讯平台连接成功
	virtual void onConnectError(int reason, const char * desc) override;		//与云通讯平台连接断开或者出错
	virtual void onIncomingCallReceived(int callType, const char *callid, const char *caller) override;  //有呼叫呼入
	virtual void onCallProceeding(const char*callid) override;		//呼叫已经被云通讯平台处理
	virtual void onCallAlerting(const char *callid) override;			//呼叫振铃
	virtual void onCallAnswered(const char *callid) override;			//外呼对方应答
	virtual void onMakeCallFailed(const char *callid, int reason) override;//外呼失败
	virtual void onCallPaused(const char* callid) override;				//本地Pause呼叫成功
	virtual void onCallPausedByRemote(const char *callid) override;		//呼叫被被叫pasue
	virtual void onCallReleased(const char *callid, int reason) override;				//呼叫挂机
	virtual void onCallTransfered(const char *callid, const char *destionation) override; //呼叫被转接
	virtual void onDtmfReceived(const char *callid, char dtmf) override;		//收到DTMF按键时的回调
	virtual void onTextMessageReceived(const char *sender, const char *receiver, const char *sendtime, const char *msgid, const char *message, const char *userdata) override;		//收到文本短消息
																																													//void (*onGroupTextMessageReceived)(const char* sender, const char* groupid, const char *message) =0; //收到群组文本短消息
	virtual void onMessageSendReport(const char*msgid, const char*time, int status) override; //发送消息结果
	virtual void onLogInfo(const char* loginfo) override; // 用于接收底层的log信息,调试出现的问题.
	virtual void onResumed(const char* callid) override;
	virtual void onNotifyGeneralEvent(const char*callid, int eventType, const char*userdata, int intdata) override;	//通用事件通知
	virtual void onCallMediaUpdateRequest(const char*callid, int request) override; // 收到对方请求的更新媒体 request：0  请求增加视频（需要响应） 1:请求删除视频（不需要响应）
	virtual void onCallMediaUpdateResponse(const char*callid, int response) override;  // 本地请求更新媒体后，更新后的媒体状态 0 有视频 1 无视频
	virtual void onDeliverVideoFrame(const char*callid, unsigned char*buf, int size, int width, int height) override; //视频通话过程中，如果请求本地视频，视频数据通过这个函数上报。视频格式是RGB24
	virtual void onRecordVoiceStatus(const char *callid, const char *fileName, int status) override; //通话录音结束或者出现错误，上报事件。filenName是上层传下的文件名。 status是录音状态：0： 成功  -1：失败，录音文件删除  -2：写文件失败，保留已经保存的录音。
	virtual void onAudioData(const char *callid, const void *inData, int inLen, void *outData, int &outLen, bool send) override; //在音频数据发送之前，将音频数据返回给上层处理，然后将上层处理后的数据返回来。
	virtual void onOriginalAudioData(const char *callid, const void *inData, int inLen, int sampleRate, int numChannels, const char *codec, bool send) override; //将原始音频数据抛到上层。
	virtual void onMessageRemoteVideoRotate(const char *degree) override;//当远端视频发生旋转时，将旋转的角度上报，degree为向左旋转的度数（0，90，180，270）。
	virtual void onRequestSpecifiedVideoFailed(const char *callid, const char *sip, int reason) override;//视频会议时，请求视频数据失败
	virtual void onStopSpecifiedVideoResponse(const char *callid, const char *sip, int response, void *window) override;//视频会议时，取消视频数据响应
	virtual void onEnableSrtp(const char *sip, bool isCaller) override;//设置srtp加密属性
	virtual void onRemoteVideoRatioChanged(const char *callid, int width, int height, bool isVideoConference, const char *sipNo) override;//远端视频媒体分辨率变化时上报
	virtual void onLogOut() override;
	virtual void oneXosipThreadStop() override;

private:
	//CPing m_Ping;
};

