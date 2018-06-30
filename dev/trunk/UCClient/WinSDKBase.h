#pragma once
#include <string>
#include <log4cplus/logger.h>
#include "CCPClient.h"
#include <mutex>
#include <json/json.h>


class CWinSDKBase
{
private:
	static void sonLogInfo(const char* loginfo);//用于接收底层的log信息,调试出现的问题.
	/* 连接回调
	* reason：100连接中，200成功，其他报错；
	* jsonSting: {"authState":"(离线，uint32,1离线)","kickoffText":"(离线提示信息，string)","connectorId":"(鉴权成功后返回连接器编号，string)","version:"(服务器消息最大版本号，uint64)","collect:"(设备信息采集上报方式，uint32,1：采集关键信息后立刻上报UA[厂商+型号]、IOS Device Token 2：采集全部信息后在WIFI下上报)","pversion":"(个人资料版本号，uint64)","softVersion":"(软件最新版本号，string)","updateMode":"(更新模式，uint32,1：手动更新 2：强制更新)","historyver":"(离线消息开始版本号，uint64)","linkid":"string 服务器连接id","offlinecall":"uint32,1有离线呼叫"}
	* autoReconnect:底层自动重连标志。-1 无效，0 不自动重连，1 自动重连
	*/
	static void sonConnect(unsigned int tcpMsgIdOut, int reason, const char *jsonString, int autoReconnect);
	static void sonLogOut(unsigned int tcpMsgIdOut, int reason);//reason：200成功，其他报错；
	static void sonWillCloseTcp(void);//将要关闭TCP的提示回调
	static void sonIncomingCallReceived(int callType, int confType, const char *callid, const char *caller);  //接到呼叫 confType：-100 sipcall点对点来电，-1 protobuf点对点来电，大于0 会议来电
	static void sonCallProceeding(const char*callid);//呼叫已经被云通讯平台处理
	static void sonCallAlerting(const char *callid);	//呼叫振铃
	static void sonCallAnswered(const char *callid);	//进入通话状态(包括主叫和被叫)。主叫接收到这个事件，表明被叫已经应答；被叫接收到这个事件，表明应答成功。
	static void sonCallReleased(const char *callid, int reason, int state, int CallEvent);	//呼叫挂机。reason：错误码; state:状态值，8外呼等待振铃，9外呼等待应答，当为8或9对应着旧呼叫失败回调;CallEvent：呼叫事件
	static void sonDtmfReceived(const char *callid, char dtmf);	//收到DTMF按键时的回调	
	static void sonCallPaused(const char* callid, int type, int reason);//通话保持。type，0 本端发起，1对端发起；reason：200成功，其他报错；
	static void sonCallResumed(const char* callid, int type, int reason);//恢复暂停的通话。type，0 本端发起，1对端发起；reason：200成功，其他报错；
	static void sonSwitchCallMediaTypeRequest(const char*callid, int video, int reason); // 收到对方更新媒体请求。 video：1  请求增加视频（需要响应） 0:请求删除视频（可以不响应，底层自动去除视频）；reason：200成功，其他报错；
	static void sonSwitchCallMediaTypeResponse(const char*callid, int video, int reason);  // 对方应答媒体状态结果。 1 有视频 0 无视频；reason：200成功，其他报错；
	static void sonRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member, const char *ip, int port);//远端视频媒体分辨率变化时上报。type : 0 点对点视频,1 会议视频,2会议共享;type>0，callid为会议id,member,ip,port有效;当为会议时，ip、port有值为老会议，无值为新会议
	static void sonAudioData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send); //在音频数据发送之前，将音频数据返回给上层处理，然后将上层处理后的数据返回来。inData，传出数据，inLen传出数据长度；outData, 处理后的数据，outLen 处理后数据长度,send 上下行
	static void sonVideoData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send); //在视频数据发送之前，将视频数据返回给上层处理，然后将上层处理后的数据返回来。inData，传出数据，inLen传出数据长度；outData, 处理后的数据，outLen 处理后数据长度,send 上下行
	static void sonAudioCaptureData(const char *callid, unsigned char* data, int length, int samples, int sampleRate, int numChannels);//原始PCM音频数据。data: 原始PCM音频数据，16位小端格式;length:长度;sample: 采样个数;sampleRate:采样率;numChannels:声道个数
	static void sonVideoCaptureData(const char *callid, unsigned char* data, int length, int width, int height, int y_stride, int uv_stride);//原始I420格式视频数据。data: 标准一帧 i420 视频数据;length:长度;with:视频宽;height: 视频高;y_stride: y步长;uv_stride: u,v步长;
	static void sonMediaDestinationChanged(const char* callid, int mediaType, const char *ip, int port, int type);//媒体目标地址变化.mediaType 0 音频，1视频；上行目标地址ip和端口port；type=1 点对点,0 服务器中转；
	static void sonNoCamera(const char *callid);//无摄像头
	static void sonNoMicRecording(const char *callid, int reason);//无麦克采集,没插麦克风报错
	static void sonCallTransfered(const char *callid, const char *destionation, int reason); //呼叫被转接 reason：202服务器Accepted,200成功，其他失败
	static void sonMeetingTransfered(const char *callid, int reason); //呼叫被转会议
#ifdef WIN32
	static void sonAudioEnergyFeedback(int averageEnergy);
#endif
	static void sonSipConnect(int reason);//SIP连接回调*reason：100连接中，200成功，403服务器认证失败，其他报错；
	static void sonSipLogOut(int reason);// SIP登出回调。reason：200成功，其他报错；

private:// 虚函数 

	virtual void onLogInfo(const char* loginfo) = 0;

	/* 连接回调
	* reason：100连接中，200成功，其他报错；
	* autoReconnect:底层自动重连标志。-1 无效，0 不自动重连，1 自动重连
	*/
	virtual void onConnect(unsigned int tcpMsgIdOut, int reason, const char *jsonString, int autoReconnect) = 0;
	virtual void onLogOut(unsigned int tcpMsgIdOut, int reason) = 0;//reason：200成功，其他报错；
	virtual void onWillCloseTcp(void) = 0;
	virtual void onIncomingCallReceived(int callType, int confType, const char *callid, const char *caller) = 0;  //接到呼叫 confType：-100 sipcall点对点来电，-1 protobuf点对点来电，大于0 会议来电
	virtual void onCallProceeding(const char*callid) = 0;//呼叫已经被云通讯平台处理
	virtual void onCallAlerting(const char *callid) = 0;	//呼叫振铃
	virtual void onCallAnswered(const char *callid) = 0;	//进入通话状态(包括主叫和被叫)。主叫接收到这个事件，表明被叫已经应答；被叫接收到这个事件，表明应答成功。
	virtual void onCallReleased(const char *callid, int reason, int state, int CallEvent) = 0;	//呼叫挂机。reason：错误码; state:状态值，8外呼等待振铃，9外呼等待应答，当为8或9对应着旧呼叫失败回调;CallEvent：呼叫事件
	virtual void onDtmfReceived(const char *callid, char dtmf) = 0;	//收到DTMF按键时的回调	
	virtual void onCallPaused(const char* callid, int type, int reason) = 0;//通话保持。type，0 本端发起，1对端发起；reason：200成功，其他报错；
	virtual void onCallResumed(const char* callid, int type, int reason) = 0;//恢复暂停的通话。type，0 本端发起，1对端发起；reason：200成功，其他报错；
	virtual void onSwitchCallMediaTypeRequest(const char*callid, int video, int reason) = 0; // 收到对方更新媒体请求。 video：1  请求增加视频（需要响应） 0:请求删除视频（可以不响应，底层自动去除视频）；reason：200成功，其他报错；
	virtual void onSwitchCallMediaTypeResponse(const char*callid, int video, int reason) = 0;  // 对方应答媒体状态结果。 1 有视频 0 无视频；reason：200成功，其他报错；
	virtual void onRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member, const char *ip, int port) = 0;//远端视频媒体分辨率变化时上报。type : 0 点对点视频,1 会议视频,2会议共享;type>0，callid为会议id,member,ip,port有效;当为会议时，ip、port有值为老会议，无值为新会议
	virtual void onAudioData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send) = 0; //在音频数据发送之前，将音频数据返回给上层处理，然后将上层处理后的数据返回来。inData，传出数据，inLen传出数据长度；outData, 处理后的数据，outLen 处理后数据长度,send 上下行
	virtual void onVideoData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send) = 0; //在视频数据发送之前，将视频数据返回给上层处理，然后将上层处理后的数据返回来。inData，传出数据，inLen传出数据长度；outData, 处理后的数据，outLen 处理后数据长度,send 上下行
	virtual void onAudioCaptureData(const char *callid, unsigned char* data, int length, int samples, int sampleRate, int numChannels) = 0;//原始PCM音频数据。data: 原始PCM音频数据，16位小端格式;length:长度;sample: 采样个数;sampleRate:采样率;numChannels:声道个数
	virtual void onVideoCaptureData(const char *callid, unsigned char* data, int length, int width, int height, int y_stride, int uv_stride) = 0    ;//原始I420格式视频数据。data: 标准一帧 i420 视频数据;length:长度;with:视频宽;height: 视频高;y_stride: y步长;uv_stride: u,v步长;
	virtual void onMediaDestinationChanged(const char* callid, int mediaType, const char *ip, int port, int type) = 0;//媒体目标地址变化.mediaType 0 音频，1视频；上行目标地址ip和端口port；type=1 点对点,0 服务器中转；
	virtual void onNoCamera(const char *callid) = 0;//无摄像头
	virtual void onNoMicRecording(const char *callid, int reason) = 0;//无麦克采集,没插麦克风报错
	virtual void onCallTransfered(const char *callid, const char *destionation, int reason) = 0; //呼叫被转接 reason：202服务器Accepted,200成功，其他失败
	virtual void onMeetingTransfered(const char *callid, int reason) = 0; //呼叫被转会议
#ifdef WIN32
	virtual void onAudioEnergyFeedback(int averageEnergy) = 0;
#endif

	/* SIP连接回调
	* reason：100连接中，200成功，403服务器认证失败，其他报错；
	*/
	virtual void onSipConnect(int reason) = 0;

	/* SIP登出回调。
	* reason：200成功，其他报错；
	*/
	virtual void onSipLogOut(int reason) = 0;

protected:
	log4cplus::Logger log;
private:
	std::string m_addr;
	LONG port = 5060;
	std::string m_voipid;
	std::string m_voippwd;

public:
	CWinSDKBase();
	~CWinSDKBase();

	static std::string getVersion();
	static int initialize();
	static int unInitialize();
	void setTraceFlag(bool enable, const char *logFileName, int level);

	void setRing(const std::string & ringfile);
	int connectToCCPServer(std::string utf8Addr, LONG port, std::string utf8voipId, std::string utf8voipPwd);
	int makeCall(std::string strCalled);
	int acceptCall(const char *callid);
	int pauseCall(const char *callid);
	int resumeCall(const char *callid);
	int releaseCall(const char *callid, int reason);
	int transferCall(const char *callid, std::string number, int type);
	int sendDTMF(const char *callid, const char dtmf);
	const char * getCurrentCall() const;
	int disConnectToCCP();
	int setCodecEnabled(int codecid, int enabled);
	bool getCodecEnabled(int codecid);
	int setMute(bool on);
	Json::Value getMicroPhoneInfo();
	int selectMicroPhone(int microphoneIndex);
	int setUserData(int type, const char *data);
	int consultTransferCall(const char *callid, const char *consultCallid, const char *destination);
	int transferMeeting(int type, const char *callid, const char *consultedCallid, const char *consultedUser);
	int setSipTransportType(int transType);
	void setSrtpEnabled(int TransportType, int cryptoType);

};

