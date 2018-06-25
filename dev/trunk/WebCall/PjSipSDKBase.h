#pragma once
#include <string>
#include <log4cplus/logger.h>
#include <pjsua2.hpp>
#include <mutex>

class CAccount :public pj::Account {
public:
	CAccount(class CPjSipSDKBase * plugin);
	~CAccount();
	virtual void onRegStarted(pj::OnRegStartedParam & prm) override;// ע���ע���Ǽ�ʱ֪ͨ����
	virtual void onRegState(pj::OnRegStateParam &prm) override;//��¼״̬�ı�ص�
	virtual void onCallState(const pj::CallInfo & ci);//ͨ��״̬�ı�ص�
	virtual void onDtmfDigit(pjsua_call_id call_id, const std::string & dtmf);
	virtual void onIncomingCall(pj::OnIncomingCallParam &prm) override;

	void makeCall(const std::string & strCalled, pj::Call ** call);
private:
	log4cplus::Logger log;
	class CPjSipSDKBase * m_Plugin = nullptr;
	std::recursive_mutex m_callsmtx;
	std::map<pjsua_call_id, pj::Call *>m_calls;
	friend class CPCall;
	friend class CPjSipSDKBase;
};

class CPjSipSDKBase
{
public:
	virtual void onRegStarted(pj::OnRegStartedParam & prm);// ע���ע���Ǽ�ʱ֪ͨ����
	virtual void onRegState(pj::OnRegStateParam &prm);//��¼״̬�ı�ص�
	virtual void onCallState(const pj::CallInfo & ci);//ͨ��״̬�ı�ص�
	virtual void onDtmfDigit(pjsua_call_id call_id, const std::string & dtmf);
	virtual void onIncomingCall(pj::Call *call);

private:// �麯��   
	virtual void onGetCapabilityToken() = 0;			//��APP�ṩ�Ļ�ȡ����token�Ļص�����
	virtual void onConnected() = 0;					//����ͨѶƽ̨���ӳɹ�
	virtual void onConnectError(int reason, const char * desc) = 0;		//����ͨѶƽ̨���ӶϿ����߳���
	virtual void onIncomingCallReceived(int callType, const char *callid, const char *caller) = 0;  //�к��к���
	virtual void onCallProceeding(const char*callied) = 0;		//�����Ѿ�����ͨѶƽ̨����
	virtual void onCallAlerting(const char *callid) = 0;			//��������
	virtual void onCallAnswered(const char *callid) = 0;			//����Է�Ӧ��
	virtual void onMakeCallFailed(const char *callid, int reason) = 0;//���ʧ��
	virtual void onCallPaused(const char* callid) = 0;				//����Pause���гɹ�
	virtual void onCallPausedByRemote(const char *callid) = 0;		//���б�����pasue
	virtual void onCallReleased(const char *callid, int reason) = 0;				//���йһ�
	virtual void onCallTransfered(const char *callid, const char *destionation) = 0; //���б�ת��
	virtual void onDtmfReceived(const char *callid, char dtmf) = 0;		//�յ�DTMF����ʱ�Ļص�
	virtual void onTextMessageReceived(const char *sender, const char *receiver, const char *sendtime, const char *msgid, const char *message, const char *userdata) = 0;		//�յ��ı�����Ϣ
																																												//void (*onGroupTextMessageReceived)(const char* sender, const char* groupid, const char *message) =0; //�յ�Ⱥ���ı�����Ϣ
	virtual void onMessageSendReport(const char*msgid, const char*time, int status) = 0; //������Ϣ���
	virtual void onLogInfo(const char* loginfo) = 0; // ���ڽ��յײ��log��Ϣ,���Գ��ֵ�����.
	virtual void onResumed(const char* callid) = 0;
	virtual void onNotifyGeneralEvent(const char*callid, int eventType, const char*userdata, int intdata) = 0;	//ͨ���¼�֪ͨ
	virtual void onCallMediaUpdateRequest(const char*callid, int request) = 0; // �յ��Է�����ĸ���ý�� request��0  ����������Ƶ����Ҫ��Ӧ�� 1:����ɾ����Ƶ������Ҫ��Ӧ��
	virtual void onCallMediaUpdateResponse(const char*callid, int response) = 0;  // �����������ý��󣬸��º��ý��״̬ 0 ����Ƶ 1 ����Ƶ
	virtual void onDeliverVideoFrame(const char*callid, unsigned char*buf, int size, int width, int height) = 0; //��Ƶͨ�������У�������󱾵���Ƶ����Ƶ����ͨ����������ϱ�����Ƶ��ʽ��RGB24
	virtual void onRecordVoiceStatus(const char *callid, const char *fileName, int status) = 0; //ͨ��¼���������߳��ִ����ϱ��¼���filenName���ϲ㴫�µ��ļ����� status��¼��״̬��0�� �ɹ�  -1��ʧ�ܣ�¼���ļ�ɾ��  -2��д�ļ�ʧ�ܣ������Ѿ������¼����
	virtual void onAudioData(const char *callid, const void *inData, int inLen, void *outData, int &outLen, bool send) = 0; //����Ƶ���ݷ���֮ǰ������Ƶ���ݷ��ظ��ϲ㴦��Ȼ���ϲ㴦�������ݷ�������
	virtual void onOriginalAudioData(const char *callid, const void *inData, int inLen, int sampleRate, int numChannels, const char *codec, bool send) = 0; //��ԭʼ��Ƶ�����׵��ϲ㡣
	virtual void onMessageRemoteVideoRotate(const char *degree) = 0;//��Զ����Ƶ������תʱ������ת�ĽǶ��ϱ���degreeΪ������ת�Ķ�����0��90��180��270����
	virtual void onRequestSpecifiedVideoFailed(const char *callid, const char *sip, int reason) = 0;//��Ƶ����ʱ��������Ƶ����ʧ��
	virtual void onStopSpecifiedVideoResponse(const char *callid, const char *sip, int response, void *window) = 0;//��Ƶ����ʱ��ȡ����Ƶ������Ӧ
	virtual void onEnableSrtp(const char *sip, bool isCaller) = 0;//����srtp��������
	virtual void onRemoteVideoRatioChanged(const char *callid, int width, int height, bool isVideoConference, const char *sipNo) = 0;//Զ����Ƶý��ֱ��ʱ仯ʱ�ϱ�
	virtual void onLogOut() = 0;
	virtual void oneXosipThreadStop() = 0;

protected:
	log4cplus::Logger log;
private:
	static class CPEndpoint * ep;
	std::string m_addr;
	LONG port = 5060;
	std::string m_voipid;
	std::string m_voippwd;
	pjsua_call_id m_callid = PJSUA_INVALID_ID;

	std::string m_ringFile;
	pj::AudioMediaPlayer * m_player = nullptr;
	
	std::string getCodecs(int type);

public:
	CAccount * m_acc = nullptr;
public:
	CPjSipSDKBase();
	~CPjSipSDKBase();

	static std::string getVersion();
	static int initialize();
	static int unInitialize();

	void setRingFile(const std::string & ringfile);
	void startRinging();
	void stopRinging();
	int connectToCCP(std::string utf8Addr, LONG port, std::string utf8voipId, std::string utf8voipPwd);
	std::string makeCall(std::string strCalled);
	int acceptCall(int callid);
	int rejectCall(int callid, int reason);
	int pauseCall(int callid);
	int resumeCall(int callid);
	int transferCall(int callid, std::string number, int type);
	int sendDTMF(int callid, const char dtmf);
	int releaseCall(int callid);
	const int getCurrentCall() const;
	int disConnectToCCP();
	int setCodecEnabled(int codecid, int enabled);
	int getCodecEnabled(int codecid);
	int setMute(bool on);

};

