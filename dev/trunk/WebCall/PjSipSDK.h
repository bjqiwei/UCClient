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
	virtual void onGetCapabilityToken() override;			//��APP�ṩ�Ļ�ȡ����token�Ļص�����
	virtual void onConnected() override;					//����ͨѶƽ̨���ӳɹ�
	virtual void onConnectError(int reason, const char * desc) override;		//����ͨѶƽ̨���ӶϿ����߳���
	virtual void onIncomingCallReceived(int callType, const char *callid, const char *caller) override;  //�к��к���
	virtual void onCallProceeding(const char*callid) override;		//�����Ѿ�����ͨѶƽ̨����
	virtual void onCallAlerting(const char *callid) override;			//��������
	virtual void onCallAnswered(const char *callid) override;			//����Է�Ӧ��
	virtual void onMakeCallFailed(const char *callid, int reason) override;//���ʧ��
	virtual void onCallPaused(const char* callid) override;				//����Pause���гɹ�
	virtual void onCallPausedByRemote(const char *callid) override;		//���б�����pasue
	virtual void onCallReleased(const char *callid, int reason) override;				//���йһ�
	virtual void onCallTransfered(const char *callid, const char *destionation) override; //���б�ת��
	virtual void onDtmfReceived(const char *callid, char dtmf) override;		//�յ�DTMF����ʱ�Ļص�
	virtual void onTextMessageReceived(const char *sender, const char *receiver, const char *sendtime, const char *msgid, const char *message, const char *userdata) override;		//�յ��ı�����Ϣ
																																													//void (*onGroupTextMessageReceived)(const char* sender, const char* groupid, const char *message) =0; //�յ�Ⱥ���ı�����Ϣ
	virtual void onMessageSendReport(const char*msgid, const char*time, int status) override; //������Ϣ���
	virtual void onLogInfo(const char* loginfo) override; // ���ڽ��յײ��log��Ϣ,���Գ��ֵ�����.
	virtual void onResumed(const char* callid) override;
	virtual void onNotifyGeneralEvent(const char*callid, int eventType, const char*userdata, int intdata) override;	//ͨ���¼�֪ͨ
	virtual void onCallMediaUpdateRequest(const char*callid, int request) override; // �յ��Է�����ĸ���ý�� request��0  ����������Ƶ����Ҫ��Ӧ�� 1:����ɾ����Ƶ������Ҫ��Ӧ��
	virtual void onCallMediaUpdateResponse(const char*callid, int response) override;  // �����������ý��󣬸��º��ý��״̬ 0 ����Ƶ 1 ����Ƶ
	virtual void onDeliverVideoFrame(const char*callid, unsigned char*buf, int size, int width, int height) override; //��Ƶͨ�������У�������󱾵���Ƶ����Ƶ����ͨ����������ϱ�����Ƶ��ʽ��RGB24
	virtual void onRecordVoiceStatus(const char *callid, const char *fileName, int status) override; //ͨ��¼���������߳��ִ����ϱ��¼���filenName���ϲ㴫�µ��ļ����� status��¼��״̬��0�� �ɹ�  -1��ʧ�ܣ�¼���ļ�ɾ��  -2��д�ļ�ʧ�ܣ������Ѿ������¼����
	virtual void onAudioData(const char *callid, const void *inData, int inLen, void *outData, int &outLen, bool send) override; //����Ƶ���ݷ���֮ǰ������Ƶ���ݷ��ظ��ϲ㴦��Ȼ���ϲ㴦�������ݷ�������
	virtual void onOriginalAudioData(const char *callid, const void *inData, int inLen, int sampleRate, int numChannels, const char *codec, bool send) override; //��ԭʼ��Ƶ�����׵��ϲ㡣
	virtual void onMessageRemoteVideoRotate(const char *degree) override;//��Զ����Ƶ������תʱ������ת�ĽǶ��ϱ���degreeΪ������ת�Ķ�����0��90��180��270����
	virtual void onRequestSpecifiedVideoFailed(const char *callid, const char *sip, int reason) override;//��Ƶ����ʱ��������Ƶ����ʧ��
	virtual void onStopSpecifiedVideoResponse(const char *callid, const char *sip, int response, void *window) override;//��Ƶ����ʱ��ȡ����Ƶ������Ӧ
	virtual void onEnableSrtp(const char *sip, bool isCaller) override;//����srtp��������
	virtual void onRemoteVideoRatioChanged(const char *callid, int width, int height, bool isVideoConference, const char *sipNo) override;//Զ����Ƶý��ֱ��ʱ仯ʱ�ϱ�
	virtual void onLogOut() override;
	virtual void oneXosipThreadStop() override;

private:
	//CPing m_Ping;
};

