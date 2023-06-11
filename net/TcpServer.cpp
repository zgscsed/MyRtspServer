/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: TcpServer.cpp
 * @Author: zgscsed
 * @Date: 2023年6月5日21:54:25
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年6月5日21:54:29
 * @Description: TcpServer 类
	tcp服务器类
 */

#include "TcpServer.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

void Setnonblocking(int fd)
{
	int opts = fcntl(fd, F_GETFL);
	if (opts < 0)
	{
		perror("fcntl(fd,GETFL)");
		exit(1);
	}
	if (fcntl(fd, F_SETFL, opts | O_NONBLOCK) < 0)
	{
		perror("fcntl(fd,SETFL,opts)");
		exit(1);
	}
}
// loop 可以用线程池
TcpServer::TcpServer(EventLoop* loop, const int port, const int threadNum)
	:serverSocket_(),
	loop_(loop),
	serverChannel_(),
	connCount_(0)
{
	// set serverSocket
	serverSocket_.SetReuseAddr();        // 复用
	serverSocket_.Bind(port);
	serverSocket_.Listen(8192);
	serverSocket_.SetNonblocking();


	// set serverChannel
	serverChannel_.SetFd(serverSocket_.GetFd());
	serverChannel_.SetReadCallback(std::bind(&TcpServer::OnNewConnection, this));
	serverChannel_.SetErrorCallback(std::bind(&TcpServer::OnConnectionError, this));
}

TcpServer::~TcpServer()
{

}

// 启动
void TcpServer::Start()
{
	serverChannel_.SetEvents(EPOLLIN | EPOLLET);
	loop_->AddChannelToEventPoller(&serverChannel_);
}

// 注册新的连接回调函数
void TcpServer::SetNewConnCallback(const Callback& cb)
{
	this->newConnectionCallback_ = cb;
}

// 注册数据回调函数, 具体业务服务器处理消息的函数
void TcpServer::SetMessageCallback(const MessageCallback& cb)
{
	this->messageCallback_ = cb;
}

// 注册数据发送完成回调函数
void TcpServer::SetSendCompleteCallback(const Callback& cb)
{
	this->sendCompleteCallback_ = cb;
}

// 连接关闭回调函数
void TcpServer::SetCloseCallback(const Callback& cb)
{
	this->closeCallback_ = cb;
}

// 连接异常回调函数
void TcpServer::SetErrorCallback(const Callback& cb)
{
	this->errorCallback_ = cb;
}

// 新连接处理函数
void TcpServer::OnNewConnection()
{
	// 1、循环调用accept， 获取新的客户端连接
	struct sockaddr_in clientaddr;
	int clientFd;
	while ((clientFd = serverSocket_.Accept(clientaddr)) > 0)
	{
		std::cout << "new client from IP: " << inet_ntoa(clientaddr.sin_addr) << ":" << ntohs(clientaddr.sin_port) << std::endl;

		if (++connCount_ >= MAXCONNECTION)
		{
			::close(clientFd);
			continue;
		}
		Setnonblocking(clientFd);

		// 分配loop线程， io使用线程池
		EventLoop* loop = eventLoopThreadPool_.GetNextLoop();

		// 创建连接， 注册业务函数
		std::shared_ptr<TcpConnection> spTcpConnection = std::make_shared<TcpConnection>(loop, clientFd, clientaddr);
		spTcpConnection->SetMessageCallback(messageCallback_);
		spTcpConnection->SetSendCompleteCallback(sendCompleteCallback_);
		spTcpConnection->SetCloseCallback(closeCallback_);
		spTcpConnection->SetErrorCallback(errorCallback_);
		spTcpConnection->SetConnectionCleanUp(std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
		{
			std::lock_guard<std::mutex> lock(mutex_);
			tcpConnList_[clientFd] = spTcpConnection;
		}

		newConnectionCallback_(spTcpConnection);

		// 事件添加最后处理
		spTcpConnection->AddChannelToLoop();
	}

}

// 移除tcp连接函数
void TcpServer::RemoveConnection(const std::shared_ptr<TcpConnection> spTcpConnection)
{
	std::lock_guard<std::mutex> lock(mutex_);
	--connCount_;

	tcpConnList_.erase(spTcpConnection->GetFd());
}

// 服务器socket的异常处理函数
void TcpServer::OnConnectionError()
{
	std::cout << "unkown event" << std::endl;
	serverSocket_.Close();
}