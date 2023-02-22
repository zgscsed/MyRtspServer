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
#include "rtspSession.h"

#define BUFSIZE 4096

class RtspServer {
public:
	RtspServer(int serverport, int rtpPort, int rtcpPort);
	~RtspServer();

	//rtsp数据接收和发送过程
	void messagesProcess(int clientSockfd);

	void start();                        //启动
private:
	TcpSocket serverSockfd_;                    //使用tcp连接，客户端和服务端
	RtspSession* session;                               //解析消息

};
#endif // !RTSPSERVER_H
