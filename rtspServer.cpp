/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp服务器类，使用rtsp协议传输数据

类功能流程：tcp接收客户端连接，根据rtsp协议接收数据并解析发送到客户端，最后传输rtp数据

*/
#include "rtspServer.h"
#include <iostream>

RtspServer::RtspServer(int serverport, int rtpPort, int rtcpPort)
	:serverSockfd_(),
	serverRtpFd_(),
	serverRtcpFd_()

{
	//设置服务器套接字
	serverSockfd_.setReuseAddr();
	serverSockfd_.bindAddr(serverport);
	serverSockfd_.listen();

	serverRtpFd_.setReuseAddr();
	serverRtpFd_.bindAddr(rtpPort);

	serverRtcpFd_.setReuseAddr();
	serverRtcpFd_.bindAddr(rtcpPort);

}
RtspServer::~RtspServer()
{
	serverSockfd_.close();
	serverRtpFd_.close();
	serverRtcpFd_.close();
}

//启动
void RtspServer::start()
{
	//第一步
	if (!serverSockfd_.isCreate() || !serverRtpFd_.isCreate() || !serverRtcpFd_.isCreate())
	{
		//创建套接字有问题，直接返回
		return;
	}

	//第二步，开始接收客户端的连接请求
	std::cout << "rtsp://" << std::endl;



}