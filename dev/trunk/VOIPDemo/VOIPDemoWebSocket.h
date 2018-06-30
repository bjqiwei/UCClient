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
	virtual void onLogInfo(const char* loginfo) override; // ���ڽ��յײ��log��Ϣ,���Գ��ֵ�����.
	virtual void onConnect(unsigned int tcpMsgIdOut, int reason, const char *jsonString, int autoReconnect) override;					//����ͨѶƽ̨���ӳɹ�
	virtual void onLogOut(unsigned int tcpMsgIdOut, int reason) override;
	virtual void onWillCloseTcp(void) override;
	virtual void onIncomingCallReceived(int callType, int confType, const char *callid, const char *caller) override;  //�к��к���
	virtual void onCallProceeding(const char*callid) override;		//�����Ѿ�����ͨѶƽ̨����
	virtual void onCallAlerting(const char *callid) override;			//��������
	virtual void onCallAnswered(const char *callid) override;			//����Է�Ӧ��
	virtual void onCallReleased(const char *callid, int reason, int state, int CallEvent) override;				//���йһ�
	virtual void onDtmfReceived(const char *callid, char dtmf) override;		//�յ�DTMF����ʱ�Ļص�
	virtual void onCallPaused(const char* callid, int type, int reason) override;				//����Pause���гɹ�
	virtual void onCallResumed(const char* callid, int type, int reason) override;
	virtual void onSwitchCallMediaTypeRequest(const char*callid, int video, int reason) override; // �յ��Է�����ý������ video��1  ����������Ƶ����Ҫ��Ӧ�� 0:����ɾ����Ƶ�����Բ���Ӧ���ײ��Զ�ȥ����Ƶ����reason��200�ɹ�����������
	virtual void onSwitchCallMediaTypeResponse(const char*callid, int video, int reason) override;  // �Է�Ӧ��ý��״̬����� 1 ����Ƶ 0 ����Ƶ��reason��200�ɹ�����������
	virtual void onRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member, const char *ip, int port) override;//Զ����Ƶý��ֱ��ʱ仯ʱ�ϱ���type : 0 ��Ե���Ƶ,1 ������Ƶ,2���鹲��;type>0��callidΪ����id,member,ip,port��Ч;��Ϊ����ʱ��ip��port��ֵΪ�ϻ��飬��ֵΪ�»���
	virtual void onAudioData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send) override; //����Ƶ���ݷ���֮ǰ������Ƶ���ݷ��ظ��ϲ㴦��Ȼ���ϲ㴦�������ݷ�������inData���������ݣ�inLen�������ݳ��ȣ�outData, ���������ݣ�outLen ��������ݳ���,send ������
	virtual void onVideoData(const char *callid, const void *inData, int inLen, void *outData, int *outLen, bool send) override; //����Ƶ���ݷ���֮ǰ������Ƶ���ݷ��ظ��ϲ㴦��Ȼ���ϲ㴦�������ݷ�������inData���������ݣ�inLen�������ݳ��ȣ�outData, ���������ݣ�outLen ��������ݳ���,send ������
	virtual void onAudioCaptureData(const char *callid, unsigned char* data, int length, int samples, int sampleRate, int numChannels) override;//ԭʼPCM��Ƶ���ݡ�data: ԭʼPCM��Ƶ���ݣ�16λС�˸�ʽ;length:����;sample: ��������;sampleRate:������;numChannels:��������
	virtual void onVideoCaptureData(const char *callid, unsigned char* data, int length, int width, int height, int y_stride, int uv_stride) override;//ԭʼI420��ʽ��Ƶ���ݡ�data: ��׼һ֡ i420 ��Ƶ����;length:����;with:��Ƶ��;height: ��Ƶ��;y_stride: y����;uv_stride: u,v����;
	virtual void onMediaDestinationChanged(const char* callid, int mediaType, const char *ip, int port, int type) override;//ý��Ŀ���ַ�仯.mediaType 0 ��Ƶ��1��Ƶ������Ŀ���ַip�Ͷ˿�port��type=1 ��Ե�,0 ��������ת��
	virtual void onNoCamera(const char *callid) override;//������ͷ
	virtual void onNoMicRecording(const char *callid, int reason) override;//����˲ɼ�,û����˷籨��
	virtual void onCallTransfered(const char *callid, const char *destionation, int reason) override; //���б�ת�� reason��202������Accepted,200�ɹ�������ʧ��
	virtual void onMeetingTransfered(const char *callid, int reason) override; //���б�ת����
	virtual void onAudioEnergyFeedback(int averageEnergy) override;
	virtual void onSipConnect(int reason) override; // reason��100�����У�200�ɹ���403��������֤ʧ�ܣ���������
	virtual void onSipLogOut(int reason) override; //* reason��200�ɹ�����������
};

class VOIPDemoWSServer :public WebSocket::WebSocketServer
{
public:
	explicit VOIPDemoWSServer(int port);
	virtual WebSocket::WebSocketClient * OnAccept(struct lws *wsi) override;
};

