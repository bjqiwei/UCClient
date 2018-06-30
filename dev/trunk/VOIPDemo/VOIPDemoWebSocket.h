#pragma once
#include "websocket.h"
#include <list>
#include <mutex>
#include "WinSDKBase.h"
#include <log4cplus/logger.h>

class VOIPDemoWSclient :public WebSocket::WebSocketClient, public CWinSDKBase
{
public:
	explicit VOIPDemoWSclient(struct lws * wsi);
	~VOIPDemoWSclient();

private:
	log4cplus::Logger log;
public:

	virtual void OnOpen() override;

	virtual void OnClose(const std::string & errorCode) override;

	virtual void OnError(const std::string & errorCode) override;

	virtual void OnSend() override;

	virtual void OnMessage(const std::string & message) override;


	virtual HWND  gethWnd() const;

private:
	virtual void onLogInfo(const char* loginfo) override; // 用于接收底层的log信息,调试出现的问题.
	virtual void onConnect(unsigned int tcpMsgIdOut, int reason, const char *jsonString, int autoReconnect) override;					//与云通讯平台连接成功
	virtual void onLogOut(unsigned int tcpMsgIdOut, int reason) override;
	virtual void onWillCloseTcp(void) override;
	virtual void onIncomingCallReceived(int callType, int confType, const char *callid, const char *caller) override;  //有呼叫呼入
	virtual void onCallProceeding(const char*callid) override;		//呼叫已经被云通讯平台处理
	virtual void onCallAlerting(const char *callid) override;			//呼叫振铃
	virtual void onCallAnswered(const char *callid) override;			//外呼对方应答
	virtual void onCallReleased(const char *callid, int reason, int state, int CallEvent) override;				//呼叫挂机
	virtual void onDtmfReceived(const char *callid, char dtmf) override;		//收到DTMF按键时的回调
	virtual void onCallPaused(const char* callid, int type, int reason) override;				//本地Pause呼叫成功
	virtual void onCallResumed(const char* callid, int type, int reason) override;
	virtual void onSwitchCallMediaTypeRequest(const char*callid, int video, int reason) override; // 收到对方更新媒体请求。 video：1  请求增加视频（需要响应） 0:请求删除视频（可以不响应，底层自动去除视频）；reason：200成功，其他报错；
	virtual void onSwitchCallMediaTypeResponse(const char*callid, int video, int reason) override;  // 对方应答媒体状态结果。 1 有视频 0 无视频；reason：200成功，其他报错；
	virtual void onRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member, const char *ip, int port) override;//远端视频媒体分辨率变化时上报。type : 0 点对点视频,1 会议视频,2会议共享;type>0，callid为会议id,member,ip,port有效;当为会议时，ip、port有值为老会议，无值为新会议
	virtual void onAudioData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send) override; //在音频数据发送之前，将音频数据返回给上层处理，然后将上层处理后的数据返回来。inData，传出数据，inLen传出数据长度；outData, 处理后的数据，outLen 处理后数据长度,send 上下行
	virtual void onVideoData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send) override; //在视频数据发送之前，将视频数据返回给上层处理，然后将上层处理后的数据返回来。inData，传出数据，inLen传出数据长度；outData, 处理后的数据，outLen 处理后数据长度,send 上下行
	virtual void onAudioCaptureData(const char *callid, unsigned char* data, int length, int samples, int sampleRate, int numChannels) override;//原始PCM音频数据。data: 原始PCM音频数据，16位小端格式;length:长度;sample: 采样个数;sampleRate:采样率;numChannels:声道个数
	virtual void onVideoCaptureData(const char *callid, unsigned char* data, int length, int width, int height, int y_stride, int uv_stride) override;//原始I420格式视频数据。data: 标准一帧 i420 视频数据;length:长度;with:视频宽;height: 视频高;y_stride: y步长;uv_stride: u,v步长;
	virtual void onMediaDestinationChanged(const char* callid, int mediaType, const char *ip, int port, int type) override;//媒体目标地址变化.mediaType 0 音频，1视频；上行目标地址ip和端口port；type=1 点对点,0 服务器中转；
	virtual void onNoCamera(const char *callid) override;//无摄像头
	virtual void onNoMicRecording(const char *callid, int reason) override;//无麦克采集,没插麦克风报错
	virtual void onCallTransfered(const char *callid, const char *destionation, int reason) override; //呼叫被转接 reason：202服务器Accepted,200成功，其他失败
	virtual void onMeetingTransfered(const char *callid, int reason) override; //呼叫被转会议
	virtual void onAudioEnergyFeedback(int averageEnergy) override;
	virtual void onSipConnect(int reason) override; // reason：100连接中，200成功，403服务器认证失败，其他报错
	virtual void onSipLogOut(int reason) override; //* reason：200成功，其他报错
};

class VOIPDemoWSServer :public WebSocket::WebSocketServer
{
public:
	explicit VOIPDemoWSServer(int port);
	virtual WebSocket::WebSocketClient * OnAccept(struct lws *wsi) override;
};

