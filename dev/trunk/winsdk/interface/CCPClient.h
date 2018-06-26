#ifndef _CCPCLIENT_H_
#define _CCPCLIENT_H_

#include "CCPCommon.h"
 #define CCPAPI
#define STDCALL //__stdcall

#ifndef __cplusplus
#ifdef __ANDROID__
typedef enum {false, true} bool;
#endif
#endif

#ifdef __cplusplus
extern "C" { 
#endif
 
//呼叫回调函数
struct _CALLBACKINTERFACE{
    
    /* 用于接收底层的log信息,调试出现的问题. */
	void (*onLogInfo)(const char* loginfo);
    
    /* 连接回调
     * reason：100连接中，200成功，其他报错；
     * jsonSting: {"authState":"(离线，uint32,1离线)","kickoffText":"(离线提示信息，string)","connectorId":"(鉴权成功后返回连接器编号，string)","version:"(服务器消息最大版本号，uint64)","collect:"(设备信息采集上报方式，uint32,1：采集关键信息后立刻上报UA[厂商+型号]、IOS Device Token 2：采集全部信息后在WIFI下上报)","pversion":"(个人资料版本号，uint64)","softVersion":"(软件最新版本号，string)","updateMode":"(更新模式，uint32,1：手动更新 2：强制更新)","historyver":"(离线消息开始版本号，uint64)","linkid":"string 服务器连接id","offlinecall":"uint32,1有离线呼叫"}
     * autoReconnect:底层自动重连标志。-1 无效，0 不自动重连，1 自动重连
     */
    void (*onConnect)(unsigned int tcpMsgIdOut, int reason, const char *jsonString,int autoReconnect);
    
    /* 登出回调。
     * reason：200成功，其他报错；
     */
	void (*onLogOut)(unsigned int tcpMsgIdOut, int reason);

	 /* 将要关闭TCP的提示回调。
     */
	void (*onWillCloseTcp)(void);
    
#if defined(__ANDROID__) || defined(__APPLE_CC__)
    /* 上报apple token
     * reason：200成功，其他报错；
     */
    void (*onReportDeviceToken)(unsigned int tcpMsgIdOut, int reason);
#endif
    
#ifdef __APPLE_CC__
    
    /* 从中间层获得ip协议栈
     * ipStack： ip协议栈类型，enum TLocalIPStack {ELocalIPStack_None = 0,ELocalIPStack_IPv4 = 1,ELocalIPStack_IPv6 = 2,ELocalIPStack_Dual = 3,};
     */
    void (*onGetIpStack)(int* ipStack);
#endif
      
	/****************************VOIP回调接口******************************/
#if defined(__ANDROID__) || defined(__APPLE_CC__)
    void (*onQueryOfflineCall)(unsigned int tcpMsgIdOut, int reason);//请求离线呼叫。reason：200成功，其他报错；
#endif
	void (*onIncomingCallReceived)(int callType, int confType,const char *callid, const char *caller);  //接到呼叫 confType：-100 sipcall点对点来电，-1 protobuf点对点来电，大于0 会议来电
	void (*onCallProceeding)(const char*callid);//呼叫已经被云通讯平台处理
	void (*onCallAlerting)(const char *callid);	//呼叫振铃
	void (*onCallAnswered)(const char *callid);	//进入通话状态(包括主叫和被叫)。主叫接收到这个事件，表明被叫已经应答；被叫接收到这个事件，表明应答成功。
	void (*onCallReleased)(const char *callid,int reason,int state,int CallEvent);	//呼叫挂机。reason：错误码; state:状态值，8外呼等待振铃，9外呼等待应答，当为8或9对应着旧呼叫失败回调;CallEvent：呼叫事件
	void (*onDtmfReceived)(const char *callid, char dtmf);	//收到DTMF按键时的回调	
	void (*onCallPaused)(const char* callid,int type,int reason);//通话保持。type，0 本端发起，1对端发起；reason：200成功，其他报错；
	void (*onCallResumed)(const char* callid,int type,int reason);//恢复暂停的通话。type，0 本端发起，1对端发起；reason：200成功，其他报错；
    void (*onSwitchCallMediaTypeRequest)(const char*callid,int video,int reason); // 收到对方更新媒体请求。 video：1  请求增加视频（需要响应） 0:请求删除视频（可以不响应，底层自动去除视频）；reason：200成功，其他报错；
    void (*onSwitchCallMediaTypeResponse)(const char*callid,int video,int reason);  // 对方应答媒体状态结果。 1 有视频 0 无视频；reason：200成功，其他报错；
	void (*onRemoteVideoRatio)(const char *CallidOrConferenceId, int width, int height, int type, const char *member,const char *ip,int port);//远端视频媒体分辨率变化时上报。type : 0 点对点视频,1 会议视频,2会议共享;type>0，callid为会议id,member,ip,port有效;当为会议时，ip、port有值为老会议，无值为新会议
	void (*onAudioData)(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send); //在音频数据发送之前，将音频数据返回给上层处理，然后将上层处理后的数据返回来。inData，传出数据，inLen传出数据长度；outData, 处理后的数据，outLen 处理后数据长度,send 上下行
	void (*onVideoData)(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send); //在视频数据发送之前，将视频数据返回给上层处理，然后将上层处理后的数据返回来。inData，传出数据，inLen传出数据长度；outData, 处理后的数据，outLen 处理后数据长度,send 上下行
	void (*onAudioCaptureData)(const char *callid, unsigned char* data, int length, int samples, int sampleRate, int numChannels);//原始PCM音频数据。data: 原始PCM音频数据，16位小端格式;length:长度;sample: 采样个数;sampleRate:采样率;numChannels:声道个数
	void (*onVideoCaptureData)(const char *callid, unsigned char* data, int length, int width, int height, int y_stride, int uv_stride);//原始I420格式视频数据。data: 标准一帧 i420 视频数据;length:长度;with:视频宽;height: 视频高;y_stride: y步长;uv_stride: u,v步长;
	void (*onMediaDestinationChanged)(const char* callid,int mediaType,const char *ip,int port,int type);//媒体目标地址变化.mediaType 0 音频，1视频；上行目标地址ip和端口port；type=1 点对点,0 服务器中转；
	void (*onNoCamera)(const char *callid);//无摄像头
	void (*onNoMicRecording)(const char *callid,int reason);//无麦克采集,没插麦克风报错
	void (*onCallTransfered)(const char *callid, const char *destionation, int reason); //呼叫被转接 reason：202服务器Accepted,200成功，其他失败
	void (*onMeetingTransfered)(const char *callid , int reason); //呼叫被转会议
#ifdef __ANDROID__
	void (*onSetAndroidAlarm)(int bEnable,  unsigned int millisecond);
#endif

#ifdef WIN32
	void(*onAudioEnergyFeedback)(int averageEnergy);
#endif
    
    /* SIP连接回调
     * reason：100连接中，200成功，403服务器认证失败，其他报错；
     */
    void (*onSipConnect)(int reason);
    
    /* SIP登出回调。
     * reason：200成功，其他报错；
     */
    void (*onSipLogOut)(int reason);
    
    
	_CALLBACKINTERFACE()
	{
		onLogInfo=NULL;
		onConnect=NULL;
		onLogOut=NULL;
		onWillCloseTcp=NULL;
#if defined(__ANDROID__) || defined(__APPLE_CC__)
        onReportDeviceToken=NULL;
#endif
        
#ifdef __APPLE_CC__
		onGetIpStack=NULL;
#endif

		//VOIP回调接口
#if defined(__ANDROID__) || defined(__APPLE_CC__)
		onQueryOfflineCall=NULL;
#endif
		onIncomingCallReceived=NULL;
		onCallProceeding=NULL;
		onCallAlerting=NULL;
		onCallAnswered=NULL;
		onCallReleased=NULL;
		onDtmfReceived=NULL;
		onCallPaused=NULL;
		onCallResumed=NULL;
		onSwitchCallMediaTypeRequest=NULL;
		onSwitchCallMediaTypeResponse=NULL;
		onRemoteVideoRatio=NULL;
		onAudioData=NULL;
		onVideoData=NULL;
		onAudioCaptureData=NULL;
		onVideoCaptureData=NULL;
		onMediaDestinationChanged=NULL;
		onNoCamera=NULL;
		onNoMicRecording=NULL;
        onCallTransfered=NULL;
        onMeetingTransfered=NULL;
#ifdef __ANDROID__
        onSetAndroidAlarm=NULL;
#endif
#ifdef WIN32
	onAudioEnergyFeedback=NULL;
#endif
        onSipConnect = NULL;
        onSipLogOut = NULL;

	}

};

typedef struct _CALLBACKINTERFACE CCallbackInterface;

     /*! @function
     ********************************************************************************
     函数名   : setTraceFlag
     功能     : 日志开关。包括日志打印回调和写日志文件
	  参数     : [IN]  enable				  : true开启，false停止,
				 [IN]  logFileName			  : 底层日志全路径，空则不写日志文件
				 [IN]  level				  : 日志级别，越大越详细
												enum {
													LOG_LEVEL_ERR	=10 ,//10以前预留给中间层日志
													LOG_LEVEL_WARNING,
													LOG_LEVEL_INFO,
													LOG_LEVEL_DEBUG,
													LOG_LEVEL_MEDIA_ERR=20,//媒体库日志
													LOG_LEVEL_MEDIA_WARNING,
													LOG_LEVEL_MEDIA_INFO,
													LOG_LEVEL_MEDIA_DEBUG,
													LOG_LEVEL_MEDIA_ALL,
													LOG_LEVEL_END=99
												};
				 [IN]  phoneSdkLogFileName    : 中间SDK层日志全路径，空则不上传日志文件
				 [IN]  phoneDemoLogFileName   : 应用层层日志全路径，空则不上传日志文件
				 [IN]  policy				  : 响应服务器日志上传策略,默认值-1
				 [IN]  timeOutStamp           : 响应服务器日志超时上传时时间戳，默认值-1
				 [IN]  DirCompressType        : 响应服务器日志上传目录压缩类型，底层将截取logFileName所在目录。默认值LOG_DIR_COMPRESS_DEFAULT=0；如果目录压缩建议LOG_DIR_COMPRESS_DELETE_LOG
											   enum {
													LOG_DIR_COMPRESS_DEFAULT	=0 ,//默认文件压缩，不目录压缩
													LOG_DIR_COMPRESS_NOT_DELETE,//目录压缩,压缩成功后不删除目录内原文件
													LOG_DIR_COMPRESS_DELETE_LOG,//目录压缩,压缩成功后删除目录内后缀log原文件
													LOG_DIR_COMPRESS_DELETE_ALL,//目录压缩,压缩成功后删除目录内所有原文件
												};
				 [IN]  int maxSizeMB            :当前日志大小达到此门限值时自动备份日志,单位兆字节。默认-1
     返回值   : 无。
     *******************************************************************************/
     CCPAPI void setTraceFlag(bool enable,const char *logFileName,int level,const char *phoneSdkLogFileName,const char *phoneDemoLogFileName,int policy,time_t timeOutStamp,int DirCompressType, int maxSizeMB);
	 /*! @function
     ********************************************************************************
     函数名   : getVersion
     功能     : 获取SDK版本信息
     返回值   : 版本信息字符串。
     说明     : 版本信息格式为："版本号#平台#ARM版本#音频开关#视频开关#编译日期 编译时间"
               版本号: 格式为X.X.X 如1.1.18
               平台:  Android、 Windows、 iOS、 Mac OS、 Linux
               ARM版本:  arm、 armv7、 armv5
               音频开关:  voice=false、 voice=true
               视频开关:  video=false、 video=true
               编译日期:  "MM DD YYYY"  如"Jan 19 2013"
               编译时间:  "hh:mm:ss"    如”08:30:23”）
     *******************************************************************************/
	CCPAPI const char* getVersion();
    
	/*! @function
	********************************************************************************
	函数名   : initialize
	功能     : 初始化CCP SDK
	参数     : [IN]  CCallbackInterface   : 回调函数指针结构
	返回值   : 是否初始化成功 0：成功； 非0失败
	*******************************************************************************/
	CCPAPI int servicecoreInitialize(CCallbackInterface *CCallbackInterface );
	
	/*! @function
	********************************************************************************
	函数名   : unInitialize
	功能     : 客户端注销
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	CCPAPI int unInitialize();

	/* @function
     ********************************************************************************
     函数名   : setServerAddress
     功能     : 临时设置服务器地址，供上层sdk临时测试用
	 参数     : [IN]  ccpsdkversion	    : (必须)sdk版本号。例如5002001表示5.2.1
				[IN]  connector_addr	: (必须)connectorServer地址。
				[IN]  connector_port	: (必须)connectorServer端口。
				[IN]  filehttp_addr		: (必须)filehttpServer地址。
				[IN]  filehttp_port		: (必须)filehttpServer端口。
				[IN]  lvs_addr			: connectorServer地址。
				[IN]  lvs_port			: connectorServer端口。
     返回值   : 0成功，非0失败 
     *******************************************************************************/
    CCPAPI int setServerAddress(int ccpsdkversion, const char *connector_addr, int connector_port, const char *filehttp_addr, int filehttp_port, const char *lvs_addr, int lvs_port);
	
	/* @function
     ********************************************************************************
     函数名   : setSdkVersion
     功能     : 设置上传到服务器的SDK版本号，服务器根据不同的版本号做前后兼容。dns模式为2时必须调用此接口，否则鉴权服务器报529999错误码
	 参数     : [IN]  ccpsdkversion	: sdk版本号。例如5002001表示5.2.1
     返回值   : 0成功，非0失败 
     *******************************************************************************/
    CCPAPI int setSdkVersion(int ccpsdkversion);

	/* @function
     ********************************************************************************
     函数名   : setServerArr 
     功能     : 读取本地服务器配置文件和上传到服务器的SDK版本号。根据默认策略，设置服务地址(connecterserver,lvsserver,fileserver),初始化后登陆之前调用
	 参数     : [IN]  ServerXmlFileName		: 输入文件全路径
				[IN]  ccpsdkversion		: 中间层sdk版本号。例如5002001表示5.2.1
				[IN]  type		            : 地址类型。0 生产；1 沙河
     返回值   : 0成功，非0失败 
     *******************************************************************************/
    CCPAPI int setServerArr(const char *ServerXmlFileName, int ccpsdkversion,int type);

	 /* @function
     ********************************************************************************
     函数名   : setTimeOutAckResp
     功能     : 设置业务超时时间
	 参数     : [IN]  ackSecond	  : 到达服务器超时时间。 单位秒
				[IN]  respSecond  : 业务成功超时时间。 单位秒
     返回值   : 0成功，非0失败 
     *******************************************************************************/
	CCPAPI int setTimeOutAckResp(int ackSecond,int respSecond);

	 /* @function
     ********************************************************************************
     函数名   : setKeepAliveTimeout
     功能     : 保存与设置根据网络类型不同的心跳包周期
	 参数     : [IN]  networktype	: 网络类型。enum {
										NETWORK_NONE,
										NETWORK_WIFI,
										NETWORK_4G,
										NETWORK_3G,
										NETWORK_GPRS,
										NETWORK_LAN,
									};
				[IN]  timesecond	: 超时时间。 单位秒
     返回值   : 0成功，非0失败 
     *******************************************************************************/
	CCPAPI void setKeepAliveTimeout(int networktype, int timesecond);

	/* @function
     ********************************************************************************
     函数名   : setKeepAliveTimeout
     功能     : 保存与设置，根据网络类型不同的TCP连接超时
	 参数     : [IN]  networktype	: 网络类型。enum {
										NETWORK_WIFI,
										NETWORK_4G,
										NETWORK_3G,
										NETWORK_GPRS,
										NETWORK_LAN,
										
									};
				[IN]  millisecond	: 超时时间。 单位毫秒
     返回值   : 0成功，非0失败 
     *******************************************************************************/
	CCPAPI void setTcpConnectTimeout(int networktype, int millisecond);

	 /* @function
     ********************************************************************************
     函数名   : setKeepAliveTimeout
     功能     : 传入网络状态变化状态
	 参数     : [IN]  networktype	 : 网络类型。enum {
										NETWORK_NONE,
										NETWORK_WIFI,
										NETWORK_4G,
										NETWORK_3G,
										NETWORK_GPRS,
										NETWORK_LAN,
										
									};
				[IN]  subtype		 : 子网络
				[IN]  ip			 : 当前网络地址
				[IN]  netId			 : 当前网络ID
				[IN]  bForceReconnect: 强制重连。默认不false,由内部网络变化自行判断
     返回值   : 0成功，非0失败 
     *******************************************************************************/
	CCPAPI void setNetworkType(int networktype,int subtype,const char* ip,const char* netId,bool bForceReconnect);

	/*! @function
    ********************************************************************************
     函数名   : setCycleKeepAlive
     功能     : 设置底层是否空闲周期发心跳包。
     参数     : [IN]  enable : true，发；false 不发。android底层默认不发，其他终端默认发
     返回值   : 0成功，非0失败 
    *******************************************************************************/
	int CCPAPI setCycleKeepAlive(bool enable);

	 /*! @function
     ********************************************************************************
     函数名   : sendKeepAlive
     功能     : 立刻发心跳包，验证核心连接状态，如果断了，触发重连事件
     *******************************************************************************/
	CCPAPI void sendKeepAlive();

	/*! @function
     ********************************************************************************
     函数名   : sendAlarmAlive
     功能     : 时钟周期触发
     *******************************************************************************/
    CCPAPI void sendAlarmAlive();

	 /*! @function
     ********************************************************************************
     函数名   : sendReconnect
     功能     : 重连
     *******************************************************************************/
	CCPAPI void sendReconnect();

	 /*! @function
     ********************************************************************************
     函数名   : iphoneFrontToBack
     功能     : 前台切后台
     *******************************************************************************/
	CCPAPI void iphoneFrontToBack();//断开连接
#ifdef __ANDROID__
	CCPAPI void androidFrontToBack(bool bBack); //android后台时,返给中间层的时钟周期统一设置为4分30秒
#endif

    CCPAPI int connectToCCPServer(const char *proxy_addr, int proxy_port, const char *account, const char *password);
    
	 /*! @function
     ********************************************************************************
     函数名   : connectToCCP
     功能     : 登录服务器。防火墙需要开通TCP上行服务器端口8085[连接服务器],8888[下载服务器],8090[上传服务器]；UDP所有端口
     参数     : [IN]  tcpMsgIdOut     : 回传流水号
				[IN]  userName        : 用户名
				[IN]  token           : 根据authtype来决定 1：apptoken 3：密码 4：MD5的sig值
				[IN]  appId           : 应用id
				[IN]  device          : 终端类型 1: Android Phone 2: iPhone  10: iPad  11: Android Pad  20: PC  (Just Allowed phone 2 PC(Pad) login)
				[IN]  imei            : 终端生成的设备唯一号(Android采用IMEI    IOS设备采用自定义规则)
				[IN]  networkType     : 网络类型1: WIFI 2: 4G  3: 3G  4: 2G 5: INTERNET  6: other
				[IN]  mode            : 用于踢出功能 1: 用户输入登录(用于踢出别的设备，应用传入1，底层成功后，重连时需要改成 2) 2：自动登录(服务器处理自己是否踢出逻辑，应用传入2，底层重连一直使用2)
				[IN]  authType        : 认证类型 1：完整认证 2：简化认证（一定时间内有效） 3:密码认证登录（带appkey，pwd，不需要sig字段）4:sig认证 5:临时密码认证登录（带appkey，pwd，不需要sig字段）
                [IN]  timestamp       : MD5Token使用的时间戳 yyyyMMddHHmmss。(可选)
                [IN]  localApp        : 本地绑定id，iOS取bundleid，android去包路径 (可选)
                [IN]  localLang       : 本地所使用语言 格式：语言_地区 (可选)
                [IN]  subDevice       : 设备信息   {厂商};{型号};{固件版本号} 移动端使用
				[IN]  atmId           : 柜员机唯一标识.jsonSting: {"atmid":[{"ip":"string ip地址","mac":"string mac地址"}]}
     返回值   : 0成功，非0失败
	 回调函数 ： void (*onConnect)(unsigned int tcpMsgIdOut, int reason, const char *jsonString);
     *******************************************************************************/
	CCPAPI int connectToCCP(unsigned int* tcpMsgIdOut, const char *userName, const char *token, const char *appId, int device, const char* imei,int networkType, int mode, int authType, const char *timestamp, const char* localApp, const char* localLang, const char* subDevice, const char* atmId);
    
#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
    /*! @function
     ********************************************************************************
     函数名   : reportDeviceToken
     功能     : 上报设备的token，用于离线推送（iOS和android<小米、华为>使用）
     参数     : [IN]  tcpMsgIdOut :   回传流水号
				[IN] deviceToken  :  远程推送token
                [IN] localLang    :  当前设备设置的语言
                [IN] subDevice    :  设备信息   {厂商};{型号};{固件版本号}
     返回值   : 0成功，非0失败
     *******************************************************************************/
    CCPAPI int reportDeviceToken(unsigned int* tcpMsgIdOut, const char* deviceToken, const char* localLang, const char* subDevice, int type);
#endif   

    /*! @function
     ********************************************************************************
     函数名   : disConnectToCCP
     功能     : 登出服务器
	 参数     : [IN]  tcpMsgIdOut : 回传流水号
     返回值   : 0成功； 非0失败
	 回调函数 ：void (*onLogOut)(unsigned int tcpMsgIdOut, int reason);//登出回调
     *******************************************************************************/
    CCPAPI int disConnectToCCP(unsigned int* tcpMsgIdOut);

	 /*! @function
     ********************************************************************************
     函数名   : getSerialNumber
     功能     : 生产流水号。供其他库用，例如协调库
     返回值   : 流水号
     *******************************************************************************/
	CCPAPI unsigned int getSerialNumber();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//VOIP 通话部分
	CCPAPI int STDCALL setAndroidObjects(void* javaVM, void* env, void* context);
	CCPAPI bool getRecordingStatus();

	/*! @function
	********************************************************************************
	函数名   : makeCall
	功能     : 发起呼叫		
	参数     : 	[IN]  callType	  : 呼叫类型enum {VOICE_CALL,//语音VoIP电话  VIDEO_CALL,//视频VoIP电话   VOICE_CALL_LANDING//语音落地电话};
				[IN]  called	  : 被叫方号码。根据呼叫类型不同，格式也不同
				VOICE_CALL_LANDING，目前支持只国内呼叫，手机号或者带区号的固话号码，例如：13912345678或者01088888888；			                                          
			    VOICE_CALL,
				VIDEO_CALL,
	返回值   : 返回值为callid,本次呼叫的唯一标识; NULL表示失败.
	回调函数 ：void (*onCallProceeding)(const char*callid);		//呼叫已经被云通讯平台处理
			   void (*onCallAlerting)(const char *callid);			//呼叫振铃
			   void (*onCallAnswered)(const char *callid);			//应答
			   void (*onCallReleased)(const char *callid);      //呼叫失败
	*******************************************************************************/
	CCPAPI int STDCALL makeCall(const char **OutCallid,int callType, const char *called);

	/*! @function
	********************************************************************************
	函数名   : acceptCall
	功能     : 应答呼入。可以选择媒体类型
	参数     : [IN]  callid	  : 当前呼叫的唯一标识
			   [IN]  type	  : 备用，目前此参数无效
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onCallAnswered)(const char *callid);			//应答
	*******************************************************************************/
	CCPAPI int STDCALL acceptCall(const char *callid, int type);

	/*! @function
	********************************************************************************
	函数名   : alertingCall
	功能     : 振铃
	参数     : [IN]  callid	  : 当前呼叫的唯一标识
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onCallAnswered)(const char *callid);			//应答
	*******************************************************************************/
	CCPAPI int STDCALL alertingCall(const char *callid);

	/*! @function
	********************************************************************************
	函数名   : releaseCall
	功能     : 挂机。二十秒没有语音流，SDK自动挂机
	参数     : [IN]  callid	  : 当前呼叫的唯一标识， 如果callid 为NULL,这代表所有呼叫.
	           [IN]	 reason	  : 释放呼叫的原因
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onCallReleased)(const char *callid);
	*******************************************************************************/
	CCPAPI int STDCALL releaseCall(const char *callid , int reason);
	/*! @function
	********************************************************************************
	函数名   : pauseCall
	功能     : 暂停呼叫，呼叫暂停以后, 本地的语音数据将不再传递到对方.
	参数     : [IN]  callid	  : 当前呼叫的唯一标识
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：	void (*onCallPaused)(const char* callid,int type,int reason);
	*******************************************************************************/
	CCPAPI int STDCALL pauseCall(const char *callid);

	/*! @function
	********************************************************************************
	函数名   : resumeCall
	功能     : 恢复暂停的呼叫
	参数     : [IN]  callid	  : 当前呼叫的唯一标识
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onResumed)(const char* callid,int type,int reason);
	*******************************************************************************/
	CCPAPI int STDCALL resumeCall(const char *callid);

	/*! @function
	********************************************************************************
	函数名   : transferCall
	功能     : 呼叫转移，盲转。不支持P2P网络的voip电话呼转
	参数     : [IN]  callid			: 当前呼叫的唯一标识
			   [IN]	 destination	: 目标号码
			   [IN]  type           : 呼转类型（预留）
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onCallTransfered)(const char *callid , const char *destionation,int reason); //呼叫被转接
	*******************************************************************************/
	CCPAPI int STDCALL transferCall(const char *callid , const char *destination, int type);

	/*! @function
	********************************************************************************
	函数名   : consultTransferCall
	功能     : 呼叫转移，咨询转。不支持P2P网络的voip电话呼转
	参数     : [IN]  callid			: 当前呼叫的唯一标识
			   [IN]  consultCallid  : 咨询呼叫的唯一标识
			   [IN]	 destination	: 目标号码	   
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onCallTransfered)(const char *callid , const char *destionation,int reason); //呼叫被转接
	*******************************************************************************/
	CCPAPI int STDCALL consultTransferCall(const char *callid, const char *consultCallid, const char *destination);

	/*! @function
	********************************************************************************
	函数名   : transferMeeting
	功能     : 呼叫转移。不支持P2P网络的voip电话呼转
	参数     : [IN]  type           : 转会议类型。0单步会议CCP_MEETING_TYPE_SINGLE_STEP，1 咨询会议CCP_MEETING_TYPE_CONSULT_TRANSFER
			   [IN]  callid			: 当前呼叫的唯一标识。第一路通话
			   [IN]  consultedCallid: 当前呼叫的唯一标识。第二路通话，及咨询通话；type==CCP_MEETING_TYPE_CONSULT_TRANSFER时有效，
			   [IN]	 consultedUser	: 第三方咨询专家。type==CCP_MEETING_TYPE_SINGLE_STEP时有效，
			   
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onMeetingTransfered)(const char *callid , int reason); //呼叫被转会议
	*******************************************************************************/
	CCPAPI int STDCALL transferMeeting(int type,const char *callid, const char *consultedCallid,const char *consultedUser);
	
	/*! @function
	********************************************************************************
	函数名   : sendDTMF
	功能     : 发送按键信息
	参数     : [IN]  callid	  : 当前呼叫的唯一标识.
			   [IN]	 dtmf	  : 一个按键值
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onDtmfReceived)(const char *callid, char dtmf);
	*******************************************************************************/
	CCPAPI int STDCALL sendDTMF(const char *callid, const char dtmf);

	/*! @function
	********************************************************************************
	函数名   : setUserData
	功能     : 设置用户自定义VOIP数据。
	参数     : [IN]  type : 类型
							enum {//setUserData类型
							USERDATA_FOR_TOKEN	=0,
							USERDATA_FOR_USER_AGENT,
							USERDATA_FOR_INVITE//发起呼叫};
			   [IN]  data : 具体字段。例如"tel=13812345678;nickname=小云;confpwd=123"
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	CCPAPI int STDCALL setUserData(int type ,const char *data);

	/*! @function
	********************************************************************************
	函数名   : getUserData
	功能     : 设置用户自定义VOIP数据。
	参数     : [IN]  type : 类型
							enum {//setUserData类型
							USERDATA_FOR_TOKEN	=0,
							USERDATA_FOR_USER_AGENT,
							USERDATA_FOR_INVITE,//发起呼叫
							USERDATA_FOR_200_OK=3,//应答};
			   [IN]  buffer :外部分配内存。
			   [IN]  buflen :外部分配内存长度。
			   [IN]  callid	: 当前呼叫的唯一标识
			   [IN]  outBuf	: 内部内存指针
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	CCPAPI int STDCALL getUserData(int type, char * buffer, int buflen,const char* callid,const char** outBuf);

	/*! @function
	********************************************************************************
	函数名   : getCurrentCall
	功能     : 获取当前通话的callid 
	参数     : 
	返回值   : 成功为 callid ，失败为NULL
	*******************************************************************************/
	CCPAPI const char* STDCALL getCurrentCall();

	/*! @function
	********************************************************************************
	函数名   : enableLoudsSpeaker
	功能     : 设置扬声器状态,
	参数     : [IN]  enable : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	CCPAPI int	STDCALL enableLoudsSpeaker(bool enable);

	/*! @function
	********************************************************************************
	函数名   : getLoudsSpeakerStatus
	功能     : 获取当前扬声器否开启状态
	参数     : 
	返回值   : true 开启； false关闭
	*******************************************************************************/
	CCPAPI bool STDCALL getLoudsSpeakerStatus();

	/*! @function
	********************************************************************************
	函数名   : setMute
	功能     : 通话过程中设置静音，自己能听到对方的声音，通话对方听不到自己的声音。
	参数     : [IN]  on : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	CCPAPI int STDCALL setMute(bool on);

	/*! @function
	********************************************************************************
	函数名   : getMuteStatus
	功能     : 获取静音状态
	参数     : 无
	返回值   :  true 开启； false关闭
	*******************************************************************************/
	CCPAPI bool STDCALL getMuteStatus();

	/*! @function
	********************************************************************************
	函数名   : resetMicCapture。当来电应答后，开始没有麦克风权限，当应用重新设置麦克风有权限后，要调用本接口
	功能     : 重新设置麦克风采集。
	参数     : [IN]  callid	: 当前呼叫的唯一标识
			   [IN]  on     : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	CCPAPI int resetMicCapture(const char *callid,bool on);

	/*! @function
	********************************************************************************
	函数名   : setSoftSpeak
	功能     : 设置应用播放器开关。开启，播放本应用对方声音；关闭，不能播放本应用对方声音，其他应用声音无影响。
	参数     : [IN]  callid	: 当前呼叫的唯一标识.当callid为空时，为全局设置
			   [IN]  on     : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	CCPAPI int STDCALL setSoftSpeak(const char *callid,bool on);

	/*! @function
	********************************************************************************
	函数名   : setLocalCamera
	功能     : 设置本端摄像头开启关闭，自己能看到对方，通话对方看不到自己。
	参数     : [IN]  callid	: 当前呼叫的唯一标识，可选。可以为空
			   [IN]  on     : 是否开启。设置全局摄像头使能标志
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	CCPAPI int STDCALL setLocalCamera(const char *callid,bool on);

    /*! @function
	********************************************************************************
	函数名   : getLocalCamera
	功能     : 获取全局摄像头使能标志
	参数     : [IN]  bOn  : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	CCPAPI int STDCALL getLocalCamera(bool* bOn);

    /*! @function
    ********************************************************************************
     函数名   : setAudioConfigEnabled
     功能     : 设置音频处理的开关
     参数     : [IN] type : 音频处理类型.  AUDIO_AGC, AUDIO_EC, AUDIO_NS
                [IN] enabled:  AGC默认关闭; EC和NS默认开启.
                [IN] mode: 各自对应的模式中的枚举值: AgcMode、EcMode、NsMode.
     返回值   :  是否成功 0：成功； 非0失败
    *****************************************************************************/
    CCPAPI int STDCALL setAudioConfigEnabled(int type, bool enabled, int mode);

	/*! @function
    ********************************************************************************
     函数名   : getAudioConfigEnabled
     功能     : 获得音频处理的开关状态
     参数     :  [IN] type : 音频处理类型. AUDIO_AGC, AUDIO_EC, AUDIO_NS
                 [OUT] enabled:  AGC默认关闭; EC和NS默认开启.
                 [OUT] mode: 各自对应的模式中的枚举值: AgcMode、EcMode、NsMode.
     返回值   :  是否成功 0：成功； 非0失败
    *****************************************************************************/
    CCPAPI int STDCALL getAudioConfigEnabled(int type, bool *enabled, int *mode);

	 /*! @function
     ********************************************************************************
     函数名   : setCodecEnabled
     功能     : 设置支持的编解码方式，默认全部都支持
     参数     : [IN]  type : 编解码类型.
                     codec_iLBC,
                     codec_G729,
                     codec_PCMU,
                     codec_PCMA,
                     codec_H264,
                     codec_SILK8K,
                [IN] enabled: 0 不支持；1 支持
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	CCPAPI  int STDCALL setCodecEnabled(int type,bool enabled);

	 /*! @function
     ********************************************************************************
     函数名   : getCodecEnabled
     功能     : 获得编解码方式是否被支持
     参数     : [IN]  type : 编解码类型.
                     codec_iLBC,
                     codec_G729,
                     codec_PCMU,
                     codec_PCMA,
                     codec_H264,
                     codec_SILK8K,
     返回值   :  true 支持；false不支持
     *******************************************************************************/
	CCPAPI  bool STDCALL getCodecEnabled(int type);

	 /********************************************************************************
     函数名   : setCodecEnabledPolicy
     功能     : 设置编解码协商策略
     参数     : [IN]  type : 策略 详见MediaCodecPolicy。默认为CODE_POLICY_DEFAULT
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	CCPAPI  int STDCALL setCodecEnabledPolicy(int type);

	/*! @function
     ********************************************************************************
     函数名   : setCodecNack
     功能     : 设置媒体流重传。打开后通话质量变好，但是延迟可能增加
     参数     : [IN] bAudioNack:音频开关,底层默认1。0关闭，1协商打开，2强制打开
                [IN] bVideoNack:视频开关,底层默认1。0关闭，1协商打开，2强制打开
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	CCPAPI  int STDCALL setCodecNack(int bAudioNack,int bVideoNack);

	 /*! @function
     ********************************************************************************
	 函数名   : getCodecNack
     功能     : 获得媒体流重传当前设置值。
     参数     : [IN] bAudioNack:音频开关
                [IN] bVideoNack:视频开关
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	CCPAPI  int STDCALL getCodecNack(int *bAudioNack,int *bVideoNack);

		/*! @function
     ********************************************************************************
     函数名   : setCodecRed
     功能     : 设置媒体流冗余。打开后通话减少丢包率，但是会增加流量
     参数     : [IN] bAudioRed:音频开关,底层默认2。0关闭，1协商打开,2只有会议才协商
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	CCPAPI  int STDCALL setCodecRed(int bAudioRed);

	 /*! @function
     ********************************************************************************
	 函数名   : getCodecRed
     功能     : 获得媒体流冗余当前设置值。
     参数     : [IN] bAudioRed:音频开关
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	CCPAPI  int STDCALL getCodecRed(int *bAudioRed);

     /*! @function
     ********************************************************************************
     函数名   : setRequestKeyFrameMode
     功能     : 设置视频发关键帧模式。
     参数     : [IN] mode:模式。默认0 底层媒体库RTCP；1 INFO模式
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	CCPAPI  int STDCALL setRequestKeyFrameMode(int mode);

	/*! @function
     ********************************************************************************
     函数名   : setLevelIdc
     功能     : 设置接收分辨率级别。根据长、宽、最大帧计算出级别
     参数     : [IN] width: 宽
	            [IN] height: 高
				[IN] fps: 最大帧
				[IN] level: 直接级别（可选）。默认-1；当(level>=10&&level<=52)，width,height,fps失效
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	CCPAPI  int STDCALL setLevelIdc(int width,int height,float fps,int level);

    /*! @function
     ********************************************************************************
     函数名   : setSDPProfile
     功能     : 设置profile标志和能力
     参数     : [IN] profileIndication:
     PROFILE_BASELINE = 66,
     PROFILE_MAIN     = 77,
     PROFILE_HIGH    = 100,
     PROFILE_HIGH10  = 110,
     PROFILE_HIGH422 = 122,
     PROFILE_HIGH444_PREDICTIVE = 244,
     [IN] profileCompatibility
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
    CCPAPI  int STDCALL setSDPProfile(int profileIndication,int profileCompatibility);
    
	 /*! @function
     ********************************************************************************
     函数名   : setProtoRouter
     功能     : 设置呼叫路由开关。
     参数     : [IN] enable:是否开启,true 开启,false 关闭。底层默认开启
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	 CCPAPI  int STDCALL setProtoRouter(bool enable);

	 /*! @function
     ********************************************************************************
     函数名   : getCallStatistics
     功能     : 获取通话中的统计数据
     参数     : [IN] type : 媒体类型. enum {VOICE_CALL,VIDEO_CALL};
                [OUT] statistics:  CallStatisticsInfo结构的统计数据
     返回值   :  是否成功 0：成功； 非0失败
     *****************************************************************************/
	CCPAPI int STDCALL getCallStatistics(const char *callid, bool bVideo,CCPMediaStatisticsInfo *statistics);

	/*! @function
     ********************************************************************************
     函数名   : getStatsReports
     功能     : 获取通话中的视频统计报告
     参数     :  [OUT] reportsJsonOut:  统计数据
							{
							"VideoSenderStatistics":[{"channelId":(通道id,uint32),"codecName":"(编码名称，string)","transmitBitrate":(码率，uint32),"width":(宽，uint32),"height":"(高，uint32)","frameRate":(帧率，uint32),"lossPercent":"(丢包率，单位%，uint32)","rttMs":(时延，单位毫秒，uint32),"ssrc":(用户媒体标识，uint32)}]，
							"VideoReceiverStatistics":[{"channelId":(通道id,uint32),"codecName":"(编码名称，string)","transmitBitrate":(码率，uint32),"width":(宽，uint32),"height":"(高，uint32)","frameRate":(帧率，uint32),"ssrc":(用户媒体标识，uint32)}]
							}		
     返回值   :  是否成功 0：成功； 非0失败
     *****************************************************************************/
	CCPAPI int STDCALL getStatsReports(const char ** reportsJsonOut);

 	 /*! @function
     ********************************************************************************
     函数名   :  setVideoView
	 功能     :  设置视频通话显示。呼叫前和应答前调用
	 参数     :  [IN] view      : 对方显示视图
				 [IN] localView : 本地显示视图
				 [IN] remoteShareView : 对方显示视图，双流通话有效。
				 [IN] localShareView  : 本地显示视图，双流通话有效。
     返回值   :  是否成功 0：成功； 非0失败 
     *********************************************************************************/
	CCPAPI int STDCALL setVideoView( void *view, void *localView,void *remoteShareView, void *localShareView);

	 /*! @function
     ********************************************************************************
     函数名   :  setVideoViewAttribute
	 功能     :  设置视频通话远端窗体大小。呼叫前和应答前调用
	 参数     :  [IN] width  : 宽
				 [IN] height : 高
     返回值   :  是否成功 0：成功； 非0失败 
     *********************************************************************************/
	CCPAPI int STDCALL setVideoViewAttribute(int width,int height);

	 /*! @function
     ********************************************************************************
     函数名   : resetVideoView
     功能     : 通话中重置窗体
     参数     : [IN] callid   : 当前呼叫的唯一标识.
                [IN] view      : 对方显示视图
				[IN] localView : 本地显示视图
     返回值   : 是否成功 0：成功； 非0失败
     *******************************************************************************/
     CCPAPI int resetVideoView(const char *callid, void* remoteView,void *localView);

	 /*! @function
     ********************************************************************************
     函数名   : setVideoBitRates
     功能     : 设置视频压缩的码流。如果调用，需要在建立视频通话前 
     参数     :  [IN] bitrates :  视频码流，单位kb/s
	 返回值   :  无
     ***************************************************************************/
    CCPAPI void STDCALL setVideoBitRates(int bitrates);

	/*! @function
     ********************************************************************************
     函数名   : setAudioCodecPacsizeScale
     功能     : 设置音频包大小倍数。如果调用，需要在建立通话前 
     参数     :  [IN] scale : 音频包大小缩放比
	 返回值   :  无
     ***************************************************************************/
    CCPAPI void STDCALL setAudioCodecPacsizeScale(float scale);
	 /*! @function
     ********************************************************************************
     函数名   : requestSwitchCallMediaType
     功能     : 更新通话中媒体类型,只支持voip点对点通话。voice <->video,注意：该接口只能在视频通话时才能调用，音频通话中调用无效
     参数     : [IN]  callid   : 当前呼叫的唯一标识.
                [IN]  video    : 1 添加视频； 0 去除视频
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数 ：对方 void (*onSwitchCallMediaTypeRequest)(const char*callid,int video); 
	            本地 void (*onSwitchCallMediaTypeResponse)(const char*callid,int video);  
     *******************************************************************************/
     CCPAPI int STDCALL requestSwitchCallMediaType(const char *callid, int video);
    
    /*! @function
     ********************************************************************************
     函数名   : responseSwitchCallMediaType
     功能     : 回复对方的更新通话中媒体类型请求，只支持voip点对点通话。当音频恢复视频请求时调用，删除视频请求时无效
     参数     : [IN]  callid   : 当前呼叫的唯一标识.
                [IN]  video    : 1 添加视频； 0 去除视频
     返回值   : 是否成功 0：成功； 非0失败
     *******************************************************************************/
     CCPAPI int STDCALL responseSwitchCallMediaType(const char *callid, int video);

	 /*! @function
     ********************************************************************************
     函数名   : getCallType
     功能     : 获取通话的类型。思科后协商，需要在通话后获取真正呼叫类型
     参数     : [IN]  callid	  : 当前呼叫的唯一标识.
     返回值   : 成功返回 enum {//呼叫类型
				VOICE_CALL=0,//语音VoIP电话
				VIDEO_CALL,//音视频VoIP电话
				VOICE_CALL_LANDING,//语音落地电话
				SHARE_CALL,//共享VoIP电话
				VOICE_SHARE_CALL,//语音和共享VoIP电话
				VIDEO_SHARE_CALL,//音视频和共享VoIP电话
				};中值; 其他值失败
     *******************************************************************************/
     CCPAPI int STDCALL getCallType(const char *callid);

	 /*! @function
     ********************************************************************************
     函数名   : getLocalVideoSnapshot
     功能     : 本地快照。
     参数     : [IN]  callid	  : 当前呼叫的唯一标识.
			    [IN]  fileName	  : 快照文件保存的全路径，含后缀名.jpg
     返回值   : 是否成功 0：成功； 非0失败
     *******************************************************************************/
     CCPAPI int STDCALL getLocalVideoSnapshot(const char *callid, const char* fileName);
	 CCPAPI int STDCALL getLocalVideoSnapshotEx(const char* callid, unsigned char **buf, unsigned int *size, unsigned int *width, unsigned int *height);

	 /*! @function
     ********************************************************************************
     函数名   : getRemoteVideoSnapshot
     功能     : 远端快照。
     参数     : [IN]  callid	  : 当前呼叫的唯一标识.
			    [IN]  fileName	  : 快照文件保存的全路径，含后缀名.jpg
     返回值   : 是否成功 0：成功； 非0失败
     *******************************************************************************/
     CCPAPI int STDCALL getRemoteVideoSnapshot(const char *callid, const char* fileName);
	 CCPAPI int getRemoteVideoSnapshotEx(const char* callid, unsigned char **buf, unsigned int *size, unsigned int *width, unsigned int *height);
	 
	  /*! @function
     ********************************************************************************
     函数名   : startRecordRemoteVideo
     功能     : 开始远端录像。目前只支持h264编码
     参数     : [IN]  callid	  : 当前呼叫的唯一标识.
			    [IN]  fileName	  : 文件保存的全路径，含后缀名.mp4
     返回值   : 是否成功 0：成功； 非0失败
     *******************************************************************************/
     CCPAPI int STDCALL startRecordRemoteVideo(const char* callid, const char* fileName);

	 /*! @function
     ********************************************************************************
     函数名   : stopRecordRemoteVideo
     功能     : 停止远端录像。
     参数     : [IN]  callid	  : 当前呼叫的唯一标识.
			    [IN]  fileName	  : 文件保存的全路径，含后缀名.mp4
     返回值   : 是否成功 0：成功； 非0失败
     *******************************************************************************/
     CCPAPI int STDCALL stopRecordRemoteVideo(const char* callid);
	 	  
	 /*! @function
     ********************************************************************************
     函数名   : startRecordLocalVideo
     功能     : 开始本端录像。
     参数     : [IN]  callid	  : 当前呼叫的唯一标识.
			    [IN]  fileName	  : 文件保存的全路径，含后缀名.mp4
     返回值   : 是否成功 0：成功； 非0失败
     *******************************************************************************/
     CCPAPI int STDCALL startRecordLocalVideo(const char* callid, const char* filename);

	 /*! @function
     ********************************************************************************
     函数名   : stopRecordLocalVideo
     功能     : 停止本端录像。
     参数     : [IN]  callid	  : 当前呼叫的唯一标识.
			    [IN]  fileName	  : 文件保存的全路径，含后缀名.mp4
     返回值   : 是否成功 0：成功； 非0失败
     *******************************************************************************/
     CCPAPI int STDCALL stopRecordLocalVideo(const char* callid);

	 /*! @function
     ********************************************************************************
     函数名   : startRecordScreen
     功能     : 通话过程中，开始录屏
     参数     : [IN]  callid : 会话id；
                [IN] filename：录屏保存的文件名；
                [IN] bitrate：调节录屏压缩码流，默认640；
				[IN] fps：录屏的帧数，默认10帧每秒；
				[IN] type: 录屏的屏幕选择， 0：主屏 1：辅屏
     返回值   : 0：成功   -1：失败
     *******************************************************************************/
	CCPAPI int startRecordScreen(const char *callid, const char *filename, int bitrate, int fps, int type);

	/*! @function
     ********************************************************************************
     函数名   : startRecordScreenEx
     功能     : 通话过程中，开始录屏
     参数     : [IN]  callid : 会话id；
                [IN] filename：录屏保存的文件名；
                [IN] bitrate：调节录屏压缩码流，默认640；
				[IN] fps：录屏的帧数，默认10帧每秒；
				[IN] type: 录屏的屏幕选择， 0：主屏 1：辅屏
				[IN] left, top, width, height :  屏幕区域
     返回值   : 0：成功   -1：失败
     *******************************************************************************/
	CCPAPI int startRecordScreenEx(const char *callid, const char *filename, int bitrate, int fps, int type, int left, int top, int width, int height);

	/*! @function
     ********************************************************************************
     函数名   : stopRecordScreen
     功能     : 停止录屏，通话结束时，会主动调用本函数。
     参数     : [IN]  callid : 会话id；
     返回值   : 0：成功   -1：失败
     *******************************************************************************/
	CCPAPI int stopRecordScreen(const char *callid);

	 /*! @function
     ********************************************************************************
     函数名   : getCameraInfo
     功能     : 查询摄像头信息。
     参数     : [IN] info :  摄像头信息CameraInfo
     返回值   : 摄像头个数
     *********************************************************************************/
	CCPAPI int STDCALL getCameraInfo(CCPCameraInfo **  info);

	 /*! @function
     ********************************************************************************
     函数名   : selectCamera
     功能     : 选择摄像头。可以在通话过程中选择；如果不调用，底层将使用系统默认摄像头
     参数     :  [IN] cameraIndex     : CameraInfo的index值。从CCPgetCameraInfo获取
				 [IN] capabilityIndex : CameraCapability的index值。范围[0,capabilityCount-1]
				 [IN] fps             : 最大帧数       
				 [IN] rotate          : 旋转的角度（enum {ROTATE_AUTO,ROTATE_0,ROTATE_90,ROTATE_180,ROTATE_270};中的值）
				 [IN] force           : 是否强制启动本SDK调用的摄像头。默认选false
				 [IN] scale           : 编码缩放，正实数。默认1.0，小于1.0缩小，大于1.0放大
     返回值   :  是否成功 0：成功； 非0失败
     *********************************************************************************/
	 CCPAPI int STDCALL selectCamera(int cameraIndex, int capabilityIndex,int fps,int rotate,bool force,float scale);

	  /*! @function
     ********************************************************************************
     函数名   : getSpeakerInfo
     功能     : 查询扬声器信息。
     参数     : [IN] info：扬声器信息CCPSpeakerInfo
     返回值   : 扬声器个数。
     ***************************************************************************/
     CCPAPI int STDCALL getSpeakerInfo(CCPSpeakerInfo **info);

	 /*! @function
     ********************************************************************************
     函数名   : selectSpeaker
     功能     : 选择扬声器。可以在通话过程中选择；如果不调用，SDK使用系统默认扬声器
     参数     : [IN] speakerIndex : SpeakerInfo的index值，从getSpeakerInfo获取
     返回值   : 是否成功 0：成功； 非0失败
     *********************************************************************************/
     CCPAPI int STDCALL selectSpeaker(int speakerIndex);

	 /*! @function
     ********************************************************************************
     函数名   : testSpeakerStart
     功能     : 测试扬声器打开。
     参数     : 
     返回值   : 是否成功 0：成功； 非0失败
     *********************************************************************************/
     CCPAPI int STDCALL testSpeakerStart(const char *filename, int interval, int ringmode);

	 /*! @function
     ********************************************************************************
     函数名   : testSpeakerStop
     功能     : 测试扬声器关闭。
     参数     : 
     返回值   : 是否成功 0：成功； 非0失败
     *********************************************************************************/
     CCPAPI int STDCALL testSpeakerStop(int ringmode);

	 /*! @function
     ********************************************************************************
     函数名   : getMicroPhoneInfo
     功能     : 查询麦克风信息。
     参数     : [IN] info :  麦克风信息CCPMicrophoneinfo
     返回值   : 麦克风个数。
     *********************************************************************************/
     CCPAPI int STDCALL getMicroPhoneInfo(CCPMicroPhoneInfo** info);

	 /*! @function
     ********************************************************************************
     函数名   : selectMicroPhone
	 功能     : 选择麦克风。可以在通话过程中选择；如果不调用，SDK使用系统默认麦克风
	 参数     : [IN] microphoneIndex : MicroPhoneInfo的index值，从getMicroPhoneInfo获取
     返回值   : 是否成功 0：成功； 非0失败
     *********************************************************************************/
     CCPAPI int STDCALL selectMicroPhone(int microphoneIndex);	

	 /*! @function
     ********************************************************************************
     函数名   : testMicroPhone
	 功能     : 麦克风测试 注意: 调用此函数之前必须调用selectMicroPhone函数选择音频驱动microphoneIndex
     返回值   : 是否成功 0：成功； 非0失败
     *********************************************************************************/
	 CCPAPI int STDCALL testMicroPhone();

	 	 /*! @function
     ********************************************************************************
     函数名   : testMicroPhoneStop
	 功能     : 麦克风测试
     返回值   : 是否成功 0：成功； 非0失败
     *********************************************************************************/
	 CCPAPI int STDCALL testMicroPhoneStop();

	 /*! @function
     ********************************************************************************
     函数名   : startPlayWaveFile
	 功能     : 开始播放音频文件
	 参数     : [IN] fileName : 声音文件，格式为.wav,采样率16000HZ,16bit,单声道
				[IN] bLoop : 是否循环播放
     返回值   : 是否成功 0：成功； 非0失败 
     *********************************************************************************/
	 CCPAPI int STDCALL startPlayWaveFile(const char *fileName,bool bLoop);

	 /*! @function
     ********************************************************************************
     函数名   : stopPlayWaveFile
	 功能     : 停止播放音频文件
	 参数     : 
     返回值   : 是否成功 0：成功； 非0失败
     *********************************************************************************/
	 CCPAPI int STDCALL stopPlayWaveFile();

     /*! @function ************************************************************************ 
	 函数名 : getNetworkStatistic
	 功能 : 获取通话的网络流量信息
	 参数 : [IN]  callid    : 当前呼叫的唯一标识。会议时，输入会议id
	        [OUT] duration  ：媒体交互的持续时间，单位秒，可能为0。不能作为计费结算时长，只是媒体流的统计累积时长；存储8字节
	        [OUT] send_total：在duration时间内，网络发送的总流量，单位字节。存储8字节
	        [OUT] recv_total：在duration时间内，网络接收的总流量，单位字节。存储8字节
	 返回值 : 是否成功 0：成功； 非0失败
	***********************************************************************/
	CCPAPI int STDCALL getNetworkStatistic(const char *callid, long long *duration, long long *sendTotalSim, long long *recvTotalSim, long long *sendTotalWifi, long long *recvTotalWifi);

    /*! @function
    ********************************************************************************
     函数名   : getSpeakerVolume
	 功能     : 获取扬声器音量值
     参数    : [IN] volume ：范围：【0~255】
     返回值   : 是否成功 0：成功； 非0失败
    *******************************************************************************/
	CCPAPI  int STDCALL getSpeakerVolume(unsigned int* volume);

	/*! @function
    ********************************************************************************
     函数名   : setSpeakerVolume
     功能     : 设置扬声器音量的默认值
     参数     : [IN] volume ：范围：【0~255】
     返回值   : 是否成功 0：成功； 非0失败
    *******************************************************************************/
	CCPAPI  int STDCALL setSpeakerVolume(unsigned int volume);

	/*! @function
    ********************************************************************************
     函数名   : getMicVolume
	 功能     : 获取麦克风音量值
     参数    : [IN] volume ：范围：【0~255】
     返回值   : 是否成功 0：成功； 非0失败
    *******************************************************************************/
	CCPAPI  int STDCALL getMicVolume(unsigned int* volume);

	/*! @function
    ********************************************************************************
     函数名   : setMicVolume
     功能     : 设置麦克风音量的默认值
     参数     : [IN] volume ：范围：【0~255】
     返回值   : 是否成功 0：成功； 非0失败
    *******************************************************************************/
	CCPAPI  int STDCALL setMicVolume(unsigned int volume);
	/*! @function
    ********************************************************************************
     函数名   : setSrtpEnabled
     功能     : 设置SRTP加密属性
     参数     : [IN]  TransportType :是否加密。0 不加密，1 加密
                [IN]  cryptoType    :加密算法，TransportType为1时有效。
                                    typedef enum  {//加密算法
									AES_128_SHA1_80 =1,
									AES_128_SHA1_32 =2,
									AES_256_SHA1_80 =3,
									AES_256_SHA1_32 =4,
								    }SrtpCryptoType;
     返回值   :无
    *******************************************************************************/
	CCPAPI void STDCALL setSrtpEnabled(int TransportType,int cryptoType);

	/*! @function
    ********************************************************************************
     函数名   : setDtxEnabled
     功能     : 舒适噪音开关，默认是关闭。关闭后没有舒适噪音，减少带宽。 
     参数     : [IN]  enabled :   true 打开; false 关闭    
     返回值   : 无 
	***************************************************************************/
   CCPAPI  void STDCALL setDtxEnabled(bool enabled);

     /*! @function
     ********************************************************************************
     函数名   : setProcessDataEnabled
     功能     : 设置允许上层处理音视频数据。一旦设置，必须把数据传回底层，否则没有媒体数据
     参数     : [IN] bAudio	  : true 允许上层处理； false 不允许上层处理.
                [IN] bVideo   : true 允许上层处理； false 不允许上层处理。
     返回值   : 成功 0 失败-1
     *******************************************************************************/
    CCPAPI int setProcessDataEnabled(bool bAudio,bool bVideo);

     /*! @function
     ********************************************************************************
     函数名   : setCaptureDataCallbackEnabled
     功能     : 设置设备采集原始音视频数据回调到上层。
     参数     : [IN] bAudio	  : 音频 true 允许； false 不允许.
                [IN] bVideo   : 视频 true 允许； false 不允许。
     返回值   : 成功 0 失败-1
     *******************************************************************************/
    CCPAPI int setCaptureDataCallbackEnabled(bool bAudio,bool bVideo);

	/*! @function
    ********************************************************************************
     函数名   : setMediaPacketTimeout
     功能     : 设置未收到对方媒体超时上报周期。 
     参数     : [IN]  secondTime: 周期，单位秒。底层默认20秒    
     返回值   : 无 
	***************************************************************************/
    CCPAPI  void setMediaPacketTimeout(int secondTime);

	/*! @function
     ********************************************************************************
     函数名   : startRecordPlayout
     功能     : 通话过程中，开始录制扬声器
     参数     : [IN]  callid : 会话id；
                [IN] filename：保存的文件名。后缀名“.wav”；
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int startRecordPlayout(const char *callid, const char *filename);

	/*! @function
     ********************************************************************************
     函数名   : stopRecordPlayout
     功能     : 停止录制扬声器。
     参数     : [IN]  callid : 会话id；
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int stopRecordPlayout(const char *callid);

	/*! @function
     ********************************************************************************
     函数名   : startRecordMicrophone
     功能     : 开始录制麦克风
     参数     : [IN] filename：保存的文件名。后缀名“.wav”；
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int startRecordMicrophone(const char *filename);

	/*! @function
     ********************************************************************************
     函数名   : stopRecordMicrophone
     功能     : 停止录制麦克风。
     参数     : 
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int stopRecordMicrophone();

	/*! @function
     ********************************************************************************
     函数名   : startRecordSendVoice
     功能     : 开始录制上行音频数据
     参数     : [IN] filename：保存的文件名。后缀名“.wav”；
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int startRecordSendVoice(const char *filename);

	/*! @function
     ********************************************************************************
     函数名   : stopRecordSendVoice
     功能     : 停止录制上行音频数据。
     参数     : 
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int stopRecordSendVoice();

	 /*! @function
     ********************************************************************************
     函数名   : startRtpDump
     功能     : 通话过程中，开始抓包。
     参数     : [IN]  callid   : 会话id；
				[IN]  mediaType:媒体类型
								enum {
									CCP_MEDIA_TYPE_AUDIO=0,//视频
									CCP_MEDIA_TYPE_VIDEO=1,//视频
									CCP_MEDIA_TYPE_SHARE=2,//共享
								};
                [IN]  fileName ：保存的文件名。后缀名“.pcap”
				[IN]  direction：方向。0下行，1上行

     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int startRtpDump(const char *callid, int mediaType, const char *fileName, int direction);

	/*! @function
     ********************************************************************************
     函数名   : stopRtpDump
     功能     : 通话过程中，停止抓包。
     参数     : [IN]  callid : 会话id；
	            [IN]  mediaType:媒体类型
								enum {
									CCP_MEDIA_TYPE_AUDIO=0,//视频
									CCP_MEDIA_TYPE_VIDEO=1,//视频
									CCP_MEDIA_TYPE_SHARE=2,//共享
								};
				[IN]  direction：方向。0下行，1上行

     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int stopRtpDump(const char *callid, int mediaType, int direction);

	/*! @function
     ********************************************************************************
     函数名   : setMagicSound
     功能     : 设置变音。
     参数     : [IN]  callid : 会话id；
				[IN]  enable : 开启与关闭;
                [IN]  pitch  ：声调调节（变调不变速），取值[-12, 12]（0表示正常，原来声音音调，设置为+8的话，可以听到明显的女生效果）
				[IN]  tempo  ：音速调节（变速不变调），取值[-50, 50], 0表示正常原语速
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int setMagicSound(const char *callid,bool enable,int pitch, int tempo);

	/*! @function
	********************************************************************************
	函数名   : setEasyMagicSound
	功能     : 设置变音简化接口。
	参数     : [IN]  callid : 会话id；
			   [IN]  enable : 开启与关闭;
			   [IN]  mode   ：模式。0：MagicSoundNormal(男变女，女变萌)
	返回值   : 0：成功   非0：失败
	*******************************************************************************/
	CCPAPI int setEasyMagicSound(const char *callid, bool enable, int mode);

	/*! @function
     ********************************************************************************
     函数名   : setBeautyFilter
     功能     : 设置美颜处理。支持通话中实时生效
     参数     : [IN] enable：是否美颜，底层默认非美颜。
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int setBeautyFilter(bool enable);

	/*! @function
     ********************************************************************************
     函数名   : setRtpKeepaliveInterval
     功能     : 设置媒体通道保活包间隔周期
     参数     : [IN] audioInterval：音频通道间隔，底层默认8秒。
				[IN] videoInterval：视频通道间隔，底层默认8秒。
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int setRtpKeepaliveInterval(int audioInterval, int videoInterval);

	/*! @function
     ********************************************************************************
     函数名   : setAudioSpeakerGain
     功能     : 设置音频输出增益。
     参数     : [IN]  callid : 会话id；
				[IN]  gain   : 大于1.0放大，小于1.0缩小；增益范围太大声音失真，建议1.5倍
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
	CCPAPI int setAudioSpeakerGain(const char *callid,float gain);

	/*! @function
	********************************************************************************
	函数名   : setAudioMicrophoneGain
	功能     : 设置音频输入增益。
	参数     : [IN]  callid : 会话id；
			   [IN]  gain   : 大于1.0放大，小于1.0缩小；增益范围太大声音失真，建议1.5倍
	返回值   : 0：成功   非0：失败
	*******************************************************************************/
	CCPAPI int setAudioMicrophoneGain(const char *callid, float gain);

	/*! @function
	********************************************************************************
	函数名   : setMTU
	功能     : 设置MTU。目前只是视频通道有效,且只支持apple和android
	参数     : [IN]  mtu :mtu值。最大不能超过1500，底层默认1200
	返回值   : 0：成功   非0：失败
	*******************************************************************************/
	CCPAPI int setMTU(int mtu);
    
#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
    /*! @function
     ********************************************************************************
     函数名: queryOfflineCall
     功能  : 请求离线呼叫
     参数  : [IN]  tcpMsgIdOut : 流水号
               [IN]  callid : 删除消息的版本号
     返回值   : 0成功，非0失败
     *******************************************************************************/
    CCPAPI int STDCALL queryOfflineCall(unsigned int* tcpMsgIdOut, const char* callid);
#endif
 
    /*! @function
     ********************************************************************************
     函数名   : setUseSipProtocol
     功能     : CALL使用SIP协议设置。
     参数     : [IN]  useSip : true使用，false不使用；
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
    CCPAPI int setUseSipProtocol(const bool useSip);
    
    /*! @function
     ********************************************************************************
     函数名   : setSipTransportType
     功能     : 设置SIP网络传输协议。
     参数     : [IN]  transport : 0 UDP，1 TCP, 2 TLS, 3 DTLS, 默认TCP；
     返回值   : 0：成功   非0：失败
     *******************************************************************************/
    CCPAPI int setSipTransportType(int transType);
    
#ifdef __cplusplus
}
#endif

#endif //_CCPCLIENT_H_
