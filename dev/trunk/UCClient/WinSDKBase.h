#pragma once
#include <string>
#include <log4cplus/logger.h>
#include "CCPClient.h"
#include <mutex>
#include <json/json.h>


class CWinSDKBase
{
private:
	static void sonLogInfo(const char* loginfo);//���ڽ��յײ��log��Ϣ,���Գ��ֵ�����.
	/* ���ӻص�
	* reason��100�����У�200�ɹ�����������
	* jsonSting: {"authState":"(���ߣ�uint32,1����)","kickoffText":"(������ʾ��Ϣ��string)","connectorId":"(��Ȩ�ɹ��󷵻���������ţ�string)","version:"(��������Ϣ���汾�ţ�uint64)","collect:"(�豸��Ϣ�ɼ��ϱ���ʽ��uint32,1���ɼ��ؼ���Ϣ�������ϱ�UA[����+�ͺ�]��IOS Device Token 2���ɼ�ȫ����Ϣ����WIFI���ϱ�)","pversion":"(�������ϰ汾�ţ�uint64)","softVersion":"(������°汾�ţ�string)","updateMode":"(����ģʽ��uint32,1���ֶ����� 2��ǿ�Ƹ���)","historyver":"(������Ϣ��ʼ�汾�ţ�uint64)","linkid":"string ����������id","offlinecall":"uint32,1�����ߺ���"}
	* autoReconnect:�ײ��Զ�������־��-1 ��Ч��0 ���Զ�������1 �Զ�����
	*/
	static void sonConnect(unsigned int tcpMsgIdOut, int reason, const char *jsonString, int autoReconnect);
	static void sonLogOut(unsigned int tcpMsgIdOut, int reason);//reason��200�ɹ�����������
	static void sonWillCloseTcp(void);//��Ҫ�ر�TCP����ʾ�ص�
	static void sonIncomingCallReceived(int callType, int confType, const char *callid, const char *caller);  //�ӵ����� confType��-100 sipcall��Ե����磬-1 protobuf��Ե����磬����0 ��������
	static void sonCallProceeding(const char*callid);//�����Ѿ�����ͨѶƽ̨����
	static void sonCallAlerting(const char *callid);	//��������
	static void sonCallAnswered(const char *callid);	//����ͨ��״̬(�������кͱ���)�����н��յ�����¼������������Ѿ�Ӧ�𣻱��н��յ�����¼�������Ӧ��ɹ���
	static void sonCallReleased(const char *callid, int reason, int state, int CallEvent);	//���йһ���reason��������; state:״ֵ̬��8����ȴ����壬9����ȴ�Ӧ�𣬵�Ϊ8��9��Ӧ�žɺ���ʧ�ܻص�;CallEvent�������¼�
	static void sonDtmfReceived(const char *callid, char dtmf);	//�յ�DTMF����ʱ�Ļص�	
	static void sonCallPaused(const char* callid, int type, int reason);//ͨ�����֡�type��0 ���˷���1�Զ˷���reason��200�ɹ�����������
	static void sonCallResumed(const char* callid, int type, int reason);//�ָ���ͣ��ͨ����type��0 ���˷���1�Զ˷���reason��200�ɹ�����������
	static void sonSwitchCallMediaTypeRequest(const char*callid, int video, int reason); // �յ��Է�����ý������ video��1  ����������Ƶ����Ҫ��Ӧ�� 0:����ɾ����Ƶ�����Բ���Ӧ���ײ��Զ�ȥ����Ƶ����reason��200�ɹ�����������
	static void sonSwitchCallMediaTypeResponse(const char*callid, int video, int reason);  // �Է�Ӧ��ý��״̬����� 1 ����Ƶ 0 ����Ƶ��reason��200�ɹ�����������
	static void sonRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member, const char *ip, int port);//Զ����Ƶý��ֱ��ʱ仯ʱ�ϱ���type : 0 ��Ե���Ƶ,1 ������Ƶ,2���鹲��;type>0��callidΪ����id,member,ip,port��Ч;��Ϊ����ʱ��ip��port��ֵΪ�ϻ��飬��ֵΪ�»���
	static void sonAudioData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send); //����Ƶ���ݷ���֮ǰ������Ƶ���ݷ��ظ��ϲ㴦��Ȼ���ϲ㴦�������ݷ�������inData���������ݣ�inLen�������ݳ��ȣ�outData, ���������ݣ�outLen ��������ݳ���,send ������
	static void sonVideoData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send); //����Ƶ���ݷ���֮ǰ������Ƶ���ݷ��ظ��ϲ㴦��Ȼ���ϲ㴦�������ݷ�������inData���������ݣ�inLen�������ݳ��ȣ�outData, ���������ݣ�outLen ��������ݳ���,send ������
	static void sonAudioCaptureData(const char *callid, unsigned char* data, int length, int samples, int sampleRate, int numChannels);//ԭʼPCM��Ƶ���ݡ�data: ԭʼPCM��Ƶ���ݣ�16λС�˸�ʽ;length:����;sample: ��������;sampleRate:������;numChannels:��������
	static void sonVideoCaptureData(const char *callid, unsigned char* data, int length, int width, int height, int y_stride, int uv_stride);//ԭʼI420��ʽ��Ƶ���ݡ�data: ��׼һ֡ i420 ��Ƶ����;length:����;with:��Ƶ��;height: ��Ƶ��;y_stride: y����;uv_stride: u,v����;
	static void sonMediaDestinationChanged(const char* callid, int mediaType, const char *ip, int port, int type);//ý��Ŀ���ַ�仯.mediaType 0 ��Ƶ��1��Ƶ������Ŀ���ַip�Ͷ˿�port��type=1 ��Ե�,0 ��������ת��
	static void sonNoCamera(const char *callid);//������ͷ
	static void sonNoMicRecording(const char *callid, int reason);//����˲ɼ�,û����˷籨��
	static void sonCallTransfered(const char *callid, const char *destionation, int reason); //���б�ת�� reason��202������Accepted,200�ɹ�������ʧ��
	static void sonMeetingTransfered(const char *callid, int reason); //���б�ת����
#ifdef WIN32
	static void sonAudioEnergyFeedback(int averageEnergy);
#endif
	static void sonSipConnect(int reason);//SIP���ӻص�*reason��100�����У�200�ɹ���403��������֤ʧ�ܣ���������
	static void sonSipLogOut(int reason);// SIP�ǳ��ص���reason��200�ɹ�����������

private:// �麯�� 

	virtual void onLogInfo(const char* loginfo) = 0;

	/* ���ӻص�
	* reason��100�����У�200�ɹ�����������
	* autoReconnect:�ײ��Զ�������־��-1 ��Ч��0 ���Զ�������1 �Զ�����
	*/
	virtual void onConnect(unsigned int tcpMsgIdOut, int reason, const char *jsonString, int autoReconnect) = 0;
	virtual void onLogOut(unsigned int tcpMsgIdOut, int reason) = 0;//reason��200�ɹ�����������
	virtual void onWillCloseTcp(void) = 0;
	virtual void onIncomingCallReceived(int callType, int confType, const char *callid, const char *caller) = 0;  //�ӵ����� confType��-100 sipcall��Ե����磬-1 protobuf��Ե����磬����0 ��������
	virtual void onCallProceeding(const char*callid) = 0;//�����Ѿ�����ͨѶƽ̨����
	virtual void onCallAlerting(const char *callid) = 0;	//��������
	virtual void onCallAnswered(const char *callid) = 0;	//����ͨ��״̬(�������кͱ���)�����н��յ�����¼������������Ѿ�Ӧ�𣻱��н��յ�����¼�������Ӧ��ɹ���
	virtual void onCallReleased(const char *callid, int reason, int state, int CallEvent) = 0;	//���йһ���reason��������; state:״ֵ̬��8����ȴ����壬9����ȴ�Ӧ�𣬵�Ϊ8��9��Ӧ�žɺ���ʧ�ܻص�;CallEvent�������¼�
	virtual void onDtmfReceived(const char *callid, char dtmf) = 0;	//�յ�DTMF����ʱ�Ļص�	
	virtual void onCallPaused(const char* callid, int type, int reason) = 0;//ͨ�����֡�type��0 ���˷���1�Զ˷���reason��200�ɹ�����������
	virtual void onCallResumed(const char* callid, int type, int reason) = 0;//�ָ���ͣ��ͨ����type��0 ���˷���1�Զ˷���reason��200�ɹ�����������
	virtual void onSwitchCallMediaTypeRequest(const char*callid, int video, int reason) = 0; // �յ��Է�����ý������ video��1  ����������Ƶ����Ҫ��Ӧ�� 0:����ɾ����Ƶ�����Բ���Ӧ���ײ��Զ�ȥ����Ƶ����reason��200�ɹ�����������
	virtual void onSwitchCallMediaTypeResponse(const char*callid, int video, int reason) = 0;  // �Է�Ӧ��ý��״̬����� 1 ����Ƶ 0 ����Ƶ��reason��200�ɹ�����������
	virtual void onRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member, const char *ip, int port) = 0;//Զ����Ƶý��ֱ��ʱ仯ʱ�ϱ���type : 0 ��Ե���Ƶ,1 ������Ƶ,2���鹲��;type>0��callidΪ����id,member,ip,port��Ч;��Ϊ����ʱ��ip��port��ֵΪ�ϻ��飬��ֵΪ�»���
	virtual void onAudioData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send) = 0; //����Ƶ���ݷ���֮ǰ������Ƶ���ݷ��ظ��ϲ㴦��Ȼ���ϲ㴦�������ݷ�������inData���������ݣ�inLen�������ݳ��ȣ�outData, ���������ݣ�outLen ��������ݳ���,send ������
	virtual void onVideoData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send) = 0; //����Ƶ���ݷ���֮ǰ������Ƶ���ݷ��ظ��ϲ㴦��Ȼ���ϲ㴦�������ݷ�������inData���������ݣ�inLen�������ݳ��ȣ�outData, ���������ݣ�outLen ��������ݳ���,send ������
	virtual void onAudioCaptureData(const char *callid, unsigned char* data, int length, int samples, int sampleRate, int numChannels) = 0;//ԭʼPCM��Ƶ���ݡ�data: ԭʼPCM��Ƶ���ݣ�16λС�˸�ʽ;length:����;sample: ��������;sampleRate:������;numChannels:��������
	virtual void onVideoCaptureData(const char *callid, unsigned char* data, int length, int width, int height, int y_stride, int uv_stride) = 0    ;//ԭʼI420��ʽ��Ƶ���ݡ�data: ��׼һ֡ i420 ��Ƶ����;length:����;with:��Ƶ��;height: ��Ƶ��;y_stride: y����;uv_stride: u,v����;
	virtual void onMediaDestinationChanged(const char* callid, int mediaType, const char *ip, int port, int type) = 0;//ý��Ŀ���ַ�仯.mediaType 0 ��Ƶ��1��Ƶ������Ŀ���ַip�Ͷ˿�port��type=1 ��Ե�,0 ��������ת��
	virtual void onNoCamera(const char *callid) = 0;//������ͷ
	virtual void onNoMicRecording(const char *callid, int reason) = 0;//����˲ɼ�,û����˷籨��
	virtual void onCallTransfered(const char *callid, const char *destionation, int reason) = 0; //���б�ת�� reason��202������Accepted,200�ɹ�������ʧ��
	virtual void onMeetingTransfered(const char *callid, int reason) = 0; //���б�ת����
#ifdef WIN32
	virtual void onAudioEnergyFeedback(int averageEnergy) = 0;
#endif

	/* SIP���ӻص�
	* reason��100�����У�200�ɹ���403��������֤ʧ�ܣ���������
	*/
	virtual void onSipConnect(int reason) = 0;

	/* SIP�ǳ��ص���
	* reason��200�ɹ�����������
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

