/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp�������࣬ʹ��rtspЭ�鴫������

�๦�����̣�tcp���տͻ������ӣ�����rtspЭ��������ݲ��������͵��ͻ��ˣ������rtp����

*/

#ifndef RTSPSERVER_H
#define RTSPSERVER_H

#include "udpSocket.h"
#include "tcpSocket.h"
#include "rtspSession.h"

#define BUFSIZE 4096

class RtspServer {
public:
	RtspServer(int serverport, int rtpPort, int rtcpPort);
	~RtspServer();

	//rtsp���ݽ��պͷ��͹���
	void messagesProcess(int clientSockfd);

	void start();                        //����
private:
	TcpSocket serverSockfd_;                    //ʹ��tcp���ӣ��ͻ��˺ͷ����
	RtspSession* session;                               //������Ϣ

};
#endif // !RTSPSERVER_H
