/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: TcpServer.cpp
 * @Author: zgscsed
 * @Date: 2023��6��5��21:54:25
 * @LastEditors: zgscsed
 * @LastEditTime: 2023��6��5��21:54:29
 * @Description: TcpServer ��
	tcp��������
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
// loop �������̳߳�
TcpServer::TcpServer(EventLoop* loop, const int port, const int threadNum)
	:serverSocket_(),
	loop_(loop),
	serverChannel_(),
	connCount_(0)
{
	// set serverSocket
	serverSocket_.SetReuseAddr();        // ����
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

// ����
void TcpServer::Start()
{
	serverChannel_.SetEvents(EPOLLIN | EPOLLET);
	loop_->AddChannelToEventPoller(&serverChannel_);
}

// ע���µ����ӻص�����
void TcpServer::SetNewConnCallback(const Callback& cb)
{
	this->newConnectionCallback_ = cb;
}

// ע�����ݻص�����, ����ҵ�������������Ϣ�ĺ���
void TcpServer::SetMessageCallback(const MessageCallback& cb)
{
	this->messageCallback_ = cb;
}

// ע�����ݷ�����ɻص�����
void TcpServer::SetSendCompleteCallback(const Callback& cb)
{
	this->sendCompleteCallback_ = cb;
}

// ���ӹرջص�����
void TcpServer::SetCloseCallback(const Callback& cb)
{
	this->closeCallback_ = cb;
}

// �����쳣�ص�����
void TcpServer::SetErrorCallback(const Callback& cb)
{
	this->errorCallback_ = cb;
}

// �����Ӵ�����
void TcpServer::OnNewConnection()
{
	// 1��ѭ������accept�� ��ȡ�µĿͻ�������
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

		// ����loop�̣߳� ioʹ���̳߳�
		EventLoop* loop = eventLoopThreadPool_.GetNextLoop();

		// �������ӣ� ע��ҵ����
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

		// �¼���������
		spTcpConnection->AddChannelToLoop();
	}

}

// �Ƴ�tcp���Ӻ���
void TcpServer::RemoveConnection(const std::shared_ptr<TcpConnection> spTcpConnection)
{
	std::lock_guard<std::mutex> lock(mutex_);
	--connCount_;

	tcpConnList_.erase(spTcpConnection->GetFd());
}

// ������socket���쳣������
void TcpServer::OnConnectionError()
{
	std::cout << "unkown event" << std::endl;
	serverSocket_.Close();
}