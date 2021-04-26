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

class RtspServer {
public:
	RtspServer(int serverport, int rtpPort, int rtcpPort);
	~RtspServer();

	void start();                        //����
private:
	TcpSocket serverSockfd_;                    //ʹ��tcp���ӣ��ͻ��˺ͷ����
	UdpSocket serverRtpFd_;                     //udp����rtp����
	UdpSocket serverRtcpFd_;                    //udp����rtcp����

};
#endif // !RTSPSERVER_H
