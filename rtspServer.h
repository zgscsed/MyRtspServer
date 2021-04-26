/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp服务器类，使用rtsp协议传输数据

类功能流程：tcp接收客户端连接，根据rtsp协议接收数据并解析发送到客户端，最后传输rtp数据

*/

#ifndef RTSPSERVER_H
#define RTSPSERVER_H

#include "udpSocket.h"
#include "tcpSocket.h"

class RtspServer {
public:
	RtspServer(int serverport, int rtpPort, int rtcpPort);
	~RtspServer();

	void start();                        //启动
private:
	TcpSocket serverSockfd_;                    //使用tcp连接，客户端和服务端
	UdpSocket serverRtpFd_;                     //udp传输rtp数据
	UdpSocket serverRtcpFd_;                    //udp传输rtcp数据

};
#endif // !RTSPSERVER_H
