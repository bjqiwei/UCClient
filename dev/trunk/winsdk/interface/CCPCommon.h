#ifndef _CCP_COMMON_H_
#define _CCP_COMMON_H_
#include <time.h>

///////////////////////////////错误码定义//////////////////////////////////////////////////////////////////////////
enum {//CCPClient err
	ERR_SDK_ALREADY_INIT =171000,//重复初始化
	ERR_NO_MEMORY=171001,//无内存
	ERR_INVALID_CALL_BACK_INTERFACE=171002,//初始化时传入的回调函数结构体为空
	ERR_SDK_UN_INIT=171003,//SDK未初始化
	ERR_NO_NETWORK=171004,//无网络连接
	ERR_NOT_SUPPORT_FUNC=171005,//接口不支持
	ERR_USER_LOGOUT_OR_UNINIT = 171006,//用户主动退出或者逆初始化
	ERR_USER_FRONT_TO_BACK = 171007,//用户主动前台切后台断开连接
};
enum {//servicecore err
	ERR_SERVICE_CORE_NULL =171030,//空指针,空内容
	ERR_SERVICE_CORE_XML=171031,//xml解析字段空指针
	ERR_SERVICE_CORE_MAX_RELOGIN_TIME_OUT=171032,//重连超过最大允许时间段，强制退出,需要外层sdk调用login
	ERR_SERVICE_CORE_PROXY_ADDR_MAP_INSERT=171033,//地址map插入错误
	ERR_SERVICE_CORE_PROXY_ADDR_MAP_EMPTY=171034,//地址map内容为空
	ERR_SERVICE_CORE_XML_WRITE=171035,//xml写字段空指针
	ERR_SERVICE_CORE_COMPRESS=171036,//压缩附件失败
	ERR_SERVICE_CORE_UNCOMPRESS=171037,//解压缩附件失败
	ERR_SERVICE_CORE_GET_ERROR_DESCRIBE=171038,//查询错误码描述失败，请稍后再查
	ERR_SERVICE_CORE_SERVER_BALANCE_DECODE_JSON=171039,//解析负载均衡json串错误
};
enum {//ProtobufCoder err
	ERR_PROTOBUF_CODER_NULL =171100,//空指针,空内容
	ERR_PROTOBUF_CODER_ENCODE_SERRIALIZE=171101,//编码序列化失败
	ERR_PROTOBUF_CODER_DECODE_READ_VARINT32=171102,//解码读取变长失败
	ERR_PROTOBUF_CODER_DECODE_MERGE=171103,//解码反序列化失败
	ERR_PROTOBUF_CODER_DECODE_CONSUMED=171104,//解码反序列化非法
};
enum {//ECserviceManage err
	ERR_EC_SERVICE_MANAGE_NULL =171130,//空指针,空内容
	ERR_EC_SERVICE_MANAGE_QUEUE_EMPTY=171131,//队列为空
	ERR_EC_SERVICE_MANAGE_CODER=171132,//编解码错误
	ERR_EC_SERVICE_MANAGE_MD5=171133,//md5错误
	ERR_EC_SERVICE_MANAGE_TIME_OUT_CHECK_INFO_MAP_INSERT=171134,//超时map插入错误
	ERR_EC_SERVICE_MANAGE_TIME_OUT_CHECK_INFO_MAP_FIND=171135,//超时map查找错误
	ERR_EC_SERVICE_MANAGE_TIME_OUT_ACK=171136,//ACK超时
	ERR_EC_SERVICE_MANAGE_TIME_OUT_RESPONSE=171137,//服务器应答超时
	ERR_EC_SERVICE_MANAGE_VERIFY_TYPE_BY_TCP_MSDID=171138,//通过流水号验证包类型
	ERR_EC_SERVICE_MANAGE_SOCKET_SEND=171139,//底层socket 发送失败
	ERR_EC_SERVICE_MANAGE_SOCKET_RESEND=171140,//底层socket 重复发送失败
	ERR_EC_SERVICE_MANAGE_SOCKET_RECEIVING_RECONNECT=171141,//接收200消息中,tcp中断重连
	ERR_EC_SERVICE_MANAGE_MSG_CONTENT_COMPRESS_UNCOMPRESS=171142,//文本内容压缩解压缩失败
	ERR_EC_SERVICE_MANAGE_SOCKET_RECEIVING_LOGIN_AGAIN=171143,//上层应用层用户输入登录（相对于用户自动登录），在等待回200消息中，再次登录报错
	ERR_EC_SERVICE_MANAGE_PUT_REQUST_MORE_FREQUENTLY=171144,//请求过于频繁
    ERR_EC_SERVICE_MANAGE_OUT_OF_RANGE=171145,//越界
	ERR_EC_SERVICE_MANAGE_GET_USER_STAT_MORE_FREQUENTLY=171146,//获得用户状态请求过于频繁
	ERR_EC_SERVICE_MANAGE_KICKOFF=171147,//被踢下线
	ERR_EC_SERVICE_MANAGE_FORCE_LOGOFF=171148,//强制下线
	ERR_EC_SERVICE_MANAGE_OUT_RANGE=171149,//参数超出界限
    ERR_EC_SERVICE_MANAGE_SET_APPLE_BADGE_MORE_FREQUENTLY=171150,//设置角标请求过于频繁
};
enum {//FileClient err
	ERR_FILE_NULL =171250,//空指针,空内容
	ERR_FILE_SYN_HTTP_REQUEST=171251,//建立socket失败或发送接收数据错误
	ERR_FILE_SYN_HTTP_STATAUS_CODE=171252,//标准协议错误。一般返回状态码为0导致的，一般是网络防火墙受限导致服务器不回包
	ERR_FILE_JSON_PARSE=171253,//JSON 解析错误
	ERR_FILE_MD5=171254,//md5错误
	ERR_FILE_MEDIA_THREAD_INFO_MAP_INSERT=171255,//插入多线程map
	ERR_FILE_MEDIA_THREAD_INFO_MAP_FIND=171256,//查找多线程map
	ERR_FILE_CREATE_FILE=171257,//创建或者打开文件错误
	ERR_FILE_ENCODE_HTTP=171258,//http封包编码错误
	ERR_FILE_CANCEL_UP_OR_DOWN_NOW=171259,//外部取消附件上传或者下载
	ERR_FILE_SEND_WSAECONNRESET=171260,//发送socket虚电路被远端复位
	ERR_FILE_BODY_NULL=171261,//发送下载请求后应答包包体为空
	ERR_FILE_HEAD_LARGE=171262,//发送下载请求后应答包头太长
	ERR_FILE_RECV_NULL=171263,//发送下载请求后应答包为空
	ERR_FILE_RECV_BLOCK=171264,//网络阻塞，超时退出
	ERR_FILE_RANGE_OUT_FILE_LENTH=171265,//断点上传偏移超过文件长度
	ERR_FILE_RANGE_OUT_FILE_OFFSET=171266,//断点下载偏移超过申请长度
	ERR_FILE_MULTIPLEX_HTTP_CLIENT_MAP_INSERT=171267,//插入复用短连接map
	ERR_FILE_MULTIPLEX_HTTP_CLIENT_MAP_FIND=171268,//查找复用短连接map
	ERR_FILE_FWRITE_FILE=171269,//写文件失败
	ERR_FILE_FIRST_READ_FILE=171270,//第一次读文件失败,可能文件大小为空
	ERR_FILE_DOWNLOAD_NOT_COMPLETE=171271,//下载文件不完整
	ERR_FILE_GET_SERVER_BALANCE_DECRYPT=171272,//查询登录地址解密失败
	ERR_FILE_RECV_REMOTE_CLOSE=171273,//接收socket被远端关闭
	ERR_FILE_SOCKS5_METHOD_REPONSE = 171274,//socks5 鉴权类型不支持
	ERR_FILE_SOCKS5_AUTHENTICATION = 171275,//socks5 鉴权失败
	ERR_FILE_SOCKS5_TRANSFER = 171276,//socks5或者http代理 中转失败
};
enum {//core tcp err
	ERR_TCP_KEEPALIVE_COUNT=171300,//心跳包计数超过上限
	ERR_TCP_KEEPALIVE_SELECT=171301,//心跳包网络检测超过
	ERR_TCP_KEEPALIVE_SEND=171302,//心跳包发送错误
	ERR_TCP_KEEPALIVE_SOCKET_NULL=171303,//心跳包socket为空
	ERR_TCP_SENDMESSAGE_CONNECT=171304,//业务包发送连接错误
	ERR_TCP_SENDMESSAGE_FIND_SOCKET=171305,//业务包发送查找socket错误
	ERR_TCP_SENDMESSAGE_SEND=171306,//业务包发送send接口错误
	ERR_TCP_READESSAGE_RECV=171307,//业务包接收错误		
	ERR_TCP_READESSAGE_SELECT=171308,//收包线程select错误
	ERR_TCP_READESSAGE_PROTOBUF_HEAD=171309,//收包解析probuf head 异常
	ERR_TCP_KEEPALIVE_SEND_CACHE_NOT_CLEAN=171310,//心跳包发送缓存未清空
	ERR_TCP_SOCKS5_METHOD_REPONSE=171311,//socks5 鉴权类型不支持
	ERR_TCP_SOCKS5_AUTHENTICATION=171312,//socks5 鉴权失败
	ERR_TCP_SOCKS5_TRANSFER=171313,//socks5或者http代理 中转失败
	ERR_TCP_READESSAGE_RECV_CLOSED_BY_REMOTE=171314,//业务包接收时服务器强制断开连接。一般是服务器黑名单保护	

};
enum {//ECcallManage err
	ERR_EC_CALL_MANAGE_NULL =171500,//空指针,空内容
	ERR_EC_CALL_MANAGE_CODER=171501,//会话协商编解码错误
	ERR_EC_CALL_MANAGE_SDP_CODER=171502,//会话协商sdp编码错误
	ERR_EC_CALL_MANAGE_USERDATA_TYPE=171503,//设置UserData类型错误
	ERR_EC_CALL_MANAGE_LOCAL_TIMER_EXPIRED=171504,//本地会话超时
	ERR_EC_CALL_MANAGE_LOCAL_INTER_ERROR=171505,//本地会话内部错误
	ERR_EC_CALL_MANAGE_SEND_TO_CONNECTOR=171506,//向connector发送请求失败
	ERR_EC_CALL_MANAGE_CAMERA_MAX_FPS=171507,//摄像头最大帧异常
	ERR_EC_CALL_MANAGE_SECOND_TALK_CONFLICT=171508,//二次协商冲突
	ERR_EC_CALL_MANAGE_FIRST_TALK_BFCP=171509,//第一次协商BFCP不支持
	ERR_EC_CALL_MANAGE_SHARE_CAPTURED=171510,//共享重复采集
	ERR_EC_CALL_MANAGE_MEDIA_PACKET_TIMEOUT_RELEASE=171511,//未收到对方媒体超时本地保护挂机
	ERR_EC_CALL_MANAGE_PROXY=171512,//代理失败
	ERR_EC_CALL_MANAGE_SDP_CODEC=171513,//协商编码失败
	ERR_EC_CALL_MANAGE_MIC_NO_AUTH=171514,//没有麦克风权限
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum {
	LOG_LEVEL_ERR	=10 ,//10以前预留给中间层日志
	LOG_LEVEL_WARNING=11,
	LOG_LEVEL_INFO=12,
	LOG_LEVEL_DEBUG=13,
	LOG_LEVEL_DEBUG_WBSS=14,//白板日志开启级别
	LOG_LEVEL_MEDIA_ERR=20,//媒体库日志,打印ECmedia调用日志和媒体库Error日志；
	LOG_LEVEL_MEDIA_WARNING=21,//添加严重错误的日志；
	LOG_LEVEL_MEDIA_INFO=22,//添加警告日志；
	LOG_LEVEL_MEDIA_DEFAULT=23,//媒体库默认日志，一般打开这个；
	LOG_LEVEL_MEDIA_DEBUG=24,//添加调试信息；
	LOG_LEVEL_MEDIA_ALL=25,//大于24： 所有日志都打开，级别最好，把媒体流的信息都打印出来
	LOG_LEVEL_END=99
};
enum {
	LOG_DIR_COMPRESS_DEFAULT	=0 ,//默认文件压缩，不目录压缩
	LOG_DIR_COMPRESS_NOT_DELETE=1,//目录压缩,压缩成功后不删除目录内原文件
	LOG_DIR_COMPRESS_DELETE_LOG=2,//目录压缩,压缩成功后删除目录内后缀log原文件
	LOG_DIR_COMPRESS_DELETE_ALL=3,//目录压缩,压缩成功后删除目录内所有原文件
};
enum {
    NETWORK_NONE=0,
    NETWORK_WIFI=1,
    NETWORK_4G=2,
    NETWORK_3G=3,   
    NETWORK_2G=4,//GPRS”和“EDGE”以及“CDMA1x”
	NETWORK_LAN=5,
	NETWORK_OTHER=6,
};

//typedef enum _CCPClientFirewallPolicy {
//    SerphonePolicyNoFirewall = 0,
//    SerphonePolicyUseIce
//    
//} CCPClientFirewallPolicy;

typedef enum {
    AUTHSTATE_NONE,
    AUTHSTATE_KICKOFF,//1. Kick Off
	AUTHSTATE_RELOGIN_MIGRATE,//2: 断线迁移 
    AUTHSTATE_IMMEDIATELY_MIGRATE,//3: 立刻迁移
	AUTHSTATE_FORCE_LOGOFF,//4强制下线
	AUTHSTATE_DNS_GET_SERVER_BALANCE,//dns查询负载均衡
}authState;

typedef enum {
    IMType_InstanceMessage=1,
    IMType_McmUserIM=2,
    IMType_McmAgentIM=3,
} IMType;

typedef enum  {
	LocalIPStack_None = 0,
	LocalIPStack_IPv4 = 1,
	LocalIPStack_IPv6 = 2,
	LocalIPStack_Dual = 3,
}LocalIPStack;
typedef enum {
	PROXYTYPE_SOCKS5 = 0,
	PROXYTYPE_HTTP   = 1,
} ProxyType;

//////////////////////////////////////MEDIA/////////////////////////////////////////////////////////////////////////
enum {//呼叫类型
	VOICE_CALL=0,//语音VoIP电话
	VIDEO_CALL=1,//音视频VoIP电话
	VOICE_CALL_LANDING=2,//语音落地电话
	SHARE_CALL=3,//共享VoIP电话
	VOICE_SHARE_CALL=4,//语音和共享VoIP电话
	VIDEO_SHARE_CALL=5,//音视频和共享VoIP电话
	POLYCOM_DOUBLE_CURRENT_CALL=6,//音视频和共享polycom双流VoIP电话,普通带Bfcp视频呼叫+二次协商
	CISCO_DOUBLE_CURRENT_CALL=7,//音视频和共享思科双流VoIP电话，一次双流协商，其他逻辑跟polycom一致
	POLYCOM_SWITCH_DOUBLE_CURRENT_CALL=8,//音视频和共享polycom双流VoIP电话,普通带Bfcp视频呼叫+共享时发起二次协商,
};

enum {//setUserData类型
	USERDATA_FOR_TOKEN	=0,
	USERDATA_FOR_USER_AGENT=1,
	USERDATA_FOR_INVITE=2,//发起呼叫
	USERDATA_FOR_200_OK=3,//应答
};
enum {//releaseCall挂机原因
	ReasonNone=0,//0一般的错误，
	ReasonDeclined=175603,//被叫拒绝应答, 当被叫为落地时，客户端不会收到此错误码，只会听到到被叫忙的播音
	ReasonCallMissed=175408,//被叫超时不应答
	ReasonBusy=175486,//被叫忙

	ReasonAuthenticationFail=175700,//700  第三方鉴权地址连接失败
	ReasonMainaccountArrearage,  //701  主账号余额不足
	ReasonMainaccountInvalid,    //702  主账号无效（未找到应用信息）
	ReasonCallRestrict,          //703  呼叫受限 ，外呼号码限制呼叫
	ReasonSameVoipId,            //704  应用未上线，仅限呼叫已配置测试号码
	ReasonSubaccountArrearage,   //705  第三方鉴权失败，子账号余额不足
	ReasonNoCalled,              //706   无被叫号码
	ReasonConferenceDismissed,   //707   呼入会议号已解散不存在
	ReasonConferencePasswordAuthentification //708   呼入会议号密码验证失败
};
enum
{
    CCP_AUDIO_AGC,
    CCP_AUDIO_EC,
    CCP_AUDIO_NS,
	CCP_AUDIO_HC,
};

enum CCPNsMode    // type of Noise Suppression
{
    NsUnchanged = 0,   // previously set mode
    NsDefault,         // platform default
    NsConference,      // conferencing default
    NsLowSuppression,  // lowest suppression
    NsModerateSuppression,
    NsHighSuppression,
    NsVeryHighSuppression,     // highest suppression
};

enum CCPAgcMode                  // type of Automatic Gain Control
{
    AgcUnchanged = 0,        // previously set mode
    AgcDefault,              // platform default
    // adaptive mode for use when analog volume control exists (e.g. for
    // PC softphone)
    AgcAdaptiveAnalog,
    // scaling takes place in the digital domain (e.g. for conference servers
    // and embedded devices)
    AgcAdaptiveDigital,
    // can be used on embedded devices where the capture signal level
    // is predictable
    AgcFixedDigital
};

// EC modes
enum CCPEcMode                  // type of Echo Control
{
    EcUnchanged = 0,          // previously set mode
    EcDefault,                // platform default
    EcConference,             // conferencing default (aggressive AEC)
    EcAec,                    // Acoustic Echo Cancellation
    EcAecm,                   // AEC mobile
};

enum  {
    codec_iLBC = 0,
    codec_G729 = 1,
    codec_PCMU = 2,
    codec_PCMA = 3,
    codec_H264 = 4,
    codec_SILK8K = 5,
    codec_AMR = 6,
    codec_VP8 = 7,
    codec_SILK16K = 8,
    codec_OPUS48 = 9,
    codec_OPUS16 = 10,
    codec_OPUS8 = 11,
	codec_H264_HIGH = 12,
};
typedef enum  {
    CODE_POLICY_DEFAULT=0,//智能选择，根据双方网络类型，选择编码协商顺序
    CODE_POLICY_QUALITY,//质量优先。opus 16k,opus 8k,pcmu,G729
    CODE_POLICY_FLOW,//省流量优先。opus 8k，G729,opus 16k，pcmu
}MediaCodecPolicy;

typedef enum  {//加密算法
	AES_128_SHA1_80 =1,
	AES_128_SHA1_32 =2,
	AES_256_SHA1_80 =3,
	AES_256_SHA1_32 =4,
}SrtpCryptoType;
enum {
	CCP_ROTATE_AUTO,
	CCP_ROTATE_0,
	CCP_ROTATE_90,
	CCP_ROTATE_180,
	CCP_ROTATE_270,
};
enum {//区分接口内外调用
	CCP_USED_EXTERNAL=0,
	CCP_USED_INTERNAL=1,
};
//申请视频源
enum {
	CCP_CONF_MEDIA_TYPE_VIDEO = 1,//视频
	CCP_CONF_MEDIA_TYPE_SHARE = 2,//共享
};
//共享上行编码缩放模式 0缩放值模式； 1固定宽高模式
typedef enum _CCP_ShareScaleMode {
	CCP_ShareScaleMode_FLOAT = 0,
	CCP_ShareScaleMode_WIDTH_HEIGHT = 1,
} CCP_ShareScaleMode;

//媒体类型，用于抓包接口
enum {
	CCP_MEDIA_TYPE_AUDIO=0,//视频
	CCP_MEDIA_TYPE_VIDEO=1,//视频
	CCP_MEDIA_TYPE_SHARE=2,//共享
};
//坐席转会议
enum {
	CCP_MEETING_TYPE_SINGLE_STEP=0,//单步会议
	CCP_MEETING_TYPE_CONSULT_TRANSFER=1,//咨询后转会议
};
//对齐与媒体库保持一致，系统默认，否则会出问题
typedef struct {//此结构体为强转为媒体库的结构体，与媒体保持一致，默认对齐
	int width;
	int height;
	int maxfps;
} CCPCameraCapability;

typedef struct{//此结构体为强转为媒体库的结构体，与媒体保持一致，默认对齐
	int index;
	char name[256];	
	char id[256];
	int capabilityCount;	
	CCPCameraCapability *capability;
} CCPCameraInfo ;
typedef struct{//此结构体为强转为媒体库的结构体，与媒体保持一致，默认对齐
	int index;
	char name[128];  //utf8
    char guid[128];	  //utf8
} CCPSpeakerInfo ;

typedef struct{//此结构体为强转为媒体库的结构体，与媒体保持一致，默认对齐
	int index;
	char name[128];   //utf8
	char guid[128];    //utf8
} CCPMicroPhoneInfo ;
	
typedef struct //此结构体为强转为媒体库的结构体，与媒体保持一致，默认对齐
{
    unsigned short fractionLost;
    unsigned int cumulativeLost;
    unsigned int extendedMax;
    unsigned int jitterSamples;
    int rttMs;
    unsigned int bytesSent;
    unsigned int packetsSent;
    unsigned int bytesReceived;
    unsigned int packetsReceived;
} CCPMediaStatisticsInfo;


typedef enum {
	Share_Device_Type_Screen =0,
	Share_Device_Type_Windows=1,
} ShareDeviceType;

#pragma pack (1) //本层定义的结构体，从媒体库拷贝的单元数据，可以一字节对齐
typedef struct{
	long long id;
	// Title of the window in UTF-8 encoding.
	char title[512];
}ShareWindowInfo;

typedef struct{
    long long id;
    // Title of the window in UTF-8 encoding.
    char title[512];
    int type;//共享设备类行0 screen; 1 windows
}ShareDeviceInfo;
#pragma pack ()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
