/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: file name
 * @Author: zgscsed
 * @Date: 2021-04-26 16:22:12
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-03-03 23:37:41
 * @Description: file content
 */
/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp服务器类，使用rtsp协议传输数据

类功能流程：tcp接收客户端连接，根据rtsp协议接收数据并解析发送到客户端，最后传输rtp数据

*/
#include "rtspServer.h"
#include "rtsp/H264Rtp.hpp"


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fcntl.h>

#include <functional>

//接收数据
int recvnn(int fd, std::string&buf)
{
	int nbyte = 0;
	int readsum = 0;
	char buffer[BUFSIZE];

	readsum = read(fd, buffer, BUFSIZE);
	if (readsum < 0)
	{
		return readsum;
	}

	buf = buffer; 
	return readsum;
}

//发送数据
int sendnn(int fd, std::string &buf)
{
	int length = buf.size();
	int nbyte = write(fd, buf.c_str(), length);

	return nbyte;
}

RtspServer::RtspServer(UsageEnvironment* env, int serverport, int rtpPort, int rtcpPort)
	:env_(env), tcpServer_(env->Scheduler()->Loop(), serverport)
{
	// tcpserver设置
	tcpServer_.SetNewConnCallback(std::bind(&RtspServer::HandleNewConnection, this, std::placeholders::_1));
	tcpServer_.SetMessageCallback(std::bind(&RtspServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
	tcpServer_.SetSendCompleteCallback(std::bind(&RtspServer::HandleSendComplete, this, std::placeholders::_1));
	tcpServer_.SetCloseCallback(std::bind(&RtspServer::HandleClose, this, std::placeholders::_1));
	tcpServer_.SetErrorCallback(std::bind(&RtspServer::HandleError, this, std::placeholders::_1));

	//session = new RtspSession(this, rtpPort, rtcpPort);
}
RtspServer::~RtspServer()
{
	//delete session;

}

//rtsp数据接收和发送过程
//void RtspServer::messagesProcess(int clientSockfd, char *clientIp)
//{
//	// RtspRequestContext rtspRequestContet;              //请求消息结构体
//	RtspMessage *rtspMessage = nullptr;
//	//RtspResponseContext rtspResponseContext;           //响应消息结构体
//	std::string rtspResponseContext;                   //响应消息
//	std::string recvBuf;                               //接收消息
//	std::string sendBuf;                               //发送消息
//
//	//rtsp交互过程
//	while (1)
//	{
//		//接收客户端的消息
//		int readsum = recvnn(clientSockfd, recvBuf);
//
//		//没有收到数据，说明连接有问题，直接退出
//		if (readsum <= 0)
//			break;
//
//		std::cout << "----------------C->S---------------------------"<<std::endl;
//		std::cout << recvBuf << std::endl;
//
//		//解析消息
//		session->praseRtspRequest(recvBuf, &rtspMessage);
//
//		//消息处理
//		session->rtspProcess(rtspMessage, rtspResponseContext);
//
//		std::cout << "----------------S->C---------------------------" << std::endl;
//		std::cout << rtspResponseContext << std::endl;
//
//
//		//返回消息
//		sendnn(clientSockfd, rtspResponseContext);
//
//		//播放
//		if (rtspMessage->rtspType == RTSP_PLAY)
//		{
//			int frameSize, startCode;
//			char* frame = (char*)malloc(500000);
//			struct RtpPacket *rtpPacket = (struct RtpPacket*)malloc(500000);
//			int fd = open("test.h264", O_RDONLY);
//			RtpHeaderInit(rtpPacket, 0, 0, 0, RTP_VESION, RTP_PAYLOAD_TYPE_H264, 0, 0, 0, 0x88923423);
//
//			std::cout <<"play"<<std::endl;
//
//			while (1)
//			{
//				frameSize = getFrameFromH264File(fd, frame, 500000);
//
//				if (frameSize < 0)
//				{
//					break;
//				}
//
//				if (startCode3(frame))
//				{
//					startCode = 3;
//				}
//				else
//				{
//					startCode = 4;
//				}
//				
//				frameSize -= startCode;
//				int ret = RtpSendH264Frame(session->serverRtpFd_.GetFd(), clientIp, session->clientRtpPort_, rtpPacket, frame+startCode, frameSize);
//				//std::cout <<"send rtp:"<< ret<<std::endl;
//				rtpPacket->rtpHeader.timestamp += 90000/25;
//
//				usleep(1000*1000/25);
//			}
//			free(frame);
//			free(rtpPacket);
//		}
//	}
//
//	//关闭客户端方向的连接
//	::close(clientSockfd);
//}

MediaSession* RtspServer::GetMediaSession(std::string name)
{
	auto iter = mediaSessions_.find(name);
	if (iter == mediaSessions_.end())
	{
		return nullptr;
	}
	return iter->second;
}

//启动
void RtspServer::start()
{
	tcpServer_.Start();

	//while (1)
	//{
	//	int clientSockfd;
	//	char *clientIP = "192.168.1.7";
	//	int clientPort;

	//	struct sockaddr_in clientaddr;

	//	//连接一个客户端后
	//	if ((clientSockfd = serverSockfd_.Accept(clientaddr)) > 0)
	//	{
	//		// 获取对端 IP 地址和端口号
	//		struct sockaddr_in peer_addr;
	//		socklen_t peer_addr_len = sizeof(peer_addr);
	//		getpeername(clientSockfd, (struct sockaddr*)&peer_addr, &peer_addr_len);

	//		// 将 IP 地址转换成字符串格式
	//		char peer_ip[INET_ADDRSTRLEN];
	//		inet_ntop(AF_INET, &peer_addr.sin_addr, peer_ip, sizeof(peer_ip));
	//		//解析客户端的ip和端口
	//		//char* clientIp = inet_ntoa(clientaddr.sin_addr);
	//		std::cout << "new clinet from ip: " << peer_ip
	//			<< ":" << ntohs(clientaddr.sin_port) << std::endl;

	//		//通信，rtsp的交互过程
	//		messagesProcess(clientSockfd, peer_ip);

	//	}
	//}
}

//新连接回调函数
void RtspServer::HandleNewConnection(const spTcpConnection& sptcpconn)
{
	// 一路rtsp会话，一个session管理
	std::shared_ptr<RtspSession> spRtspSession = std::make_shared<RtspSession>();
	// 存入map
	{
		std::lock_guard<std::mutex> lock(mutex_);
		rtspSessionMap_[sptcpconn] = spRtspSession;
		spRtspSession->SetPeerAddr(sptcpconn->GetPeerAddr());
	}
}

//数据接收回调函数
void RtspServer::HandleMessage(const spTcpConnection& sptcpconn, std::string& msg)
{
	std::shared_ptr<RtspSession> spRtspSession;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		spRtspSession = rtspSessionMap_[sptcpconn];
	}
	// 存在线程池的话，线程池中处理
	if (env_->CurThreadPool())
	{
		// 	// RtspRequestContext rtspRequestContet;              //请求消息结构体
		RtspMessage* rtspMessage = nullptr;

		//解析消息
		spRtspSession->praseRtspRequest(msg, &rtspMessage);
		// 消息异常的处理：
		
		// 设置当前的处理异步的，避免这个时候链接关闭了， 处理完后设置位false。
		sptcpconn->SetAsyncProcessing(true);
		env_->CurThreadPool()->AddTask([=]() {
			//RtspResponseContext rtspResponseContext;           //响应消息结构体
			std::string rtspResponseContext;                   //响应消息
			//消息处理
			spRtspSession->rtspProcess(rtspMessage, rtspResponseContext);

			sptcpconn->Send(rtspResponseContext);
		});

		return;
	}
	// 当前io线程处理， 不到线程池中处理
	// 	// RtspRequestContext rtspRequestContet;              //请求消息结构体
	RtspMessage* rtspMessage = nullptr;
	//RtspResponseContext rtspResponseContext;           //响应消息结构体
	std::string rtspResponseContext;                   //响应消息
	//解析消息
	spRtspSession->praseRtspRequest(msg, &rtspMessage);
	// 消息异常的处理：
	

	//消息处理
	spRtspSession->rtspProcess(rtspMessage, rtspResponseContext);

	sptcpconn->Send(rtspResponseContext);
}

//数据发送完成回调函数
void RtspServer::HandleSendComplete(const spTcpConnection& sptcpconn)
{
	// 暂时不需要
}

//连接关闭回调函数
void RtspServer::HandleClose(const spTcpConnection& sptcpconn)
{
	// 链接结束，map中删除
	std::lock_guard<std::mutex> lock(mutex_);
	rtspSessionMap_.erase(sptcpconn);
}

//连接异常回调函数
void RtspServer::HandleError(const spTcpConnection& sptcpconn)
{
	// 链接报错，map中删除
	std::lock_guard<std::mutex> lock(mutex_);
	rtspSessionMap_.erase(sptcpconn);
}