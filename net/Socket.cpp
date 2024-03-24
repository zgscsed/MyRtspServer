/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: Socket.cpp
 * @Author: zgscsed
 * @Date: 2021-04-25 13:55:09
 * @LastEditors: zgscsed
 * @LastEditTime: 2023��4��16��23:14:33
 * @Description: socket ����
	�ڻ��� Socket �ж�����һЩ���õ� socket ����������
	���� bind()��listen()��accept()��connect()��send()��recv() �� setsockopt() �ȡ�
	ͬʱ���ڻ����ж�����һ�� fd_ ���������ڱ��� socket ��������
 */

#include "Socket.hpp"
#include <iostream>

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <cstring>

Socket::Socket()
{
	fd_ = -1;
}
Socket::Socket(int domain, int type, int protocol)
{
	//�����׽���
	fd_ = socket(domain, type, protocol);
	if (fd_ == -1)
	{
		std::cout << "socket error" << std::endl;
	}
	else
		std::cout << "server socket create " << fd_ << std::endl;
}
Socket::~Socket()
{
	Close();
	std::cout << "server socket ~Socket..." << std::endl;
}
// ����fd
bool Socket::Create(int domain, int type, int protocol)
{
	//�����׽���
	fd_ = socket(domain, type, protocol);
	if (fd_ == -1)
	{
		std::cout << "socket error" << std::endl;
		return false;
	}
	else
		std::cout << "server socket create " << fd_ << std::endl;
	return true;
}
int Socket::Bind(int port)
{
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	int res = bind(fd_, (struct  sockaddr*)&serveraddr, sizeof(serveraddr));
	if (res == -1)
	{
		Close();
		perror("error bind");
		exit(1);
	}
	std::cout << "server bindaddress..." << std::endl;

	return res;
}
int Socket::Bind(const struct sockaddr* addr, socklen_t addrlen)
{
	int res = bind(fd_, addr, addrlen);
	std::cout << "server bindaddress..." << std::endl;
	if (res == -1)
	{
		Close();
		perror("error bind");
	}

	return res;
}
int Socket::Listen(int backlog)
{
	int res = ::listen(fd_, backlog);
	std::cout << "server listenning..." << std::endl;
	if (res == -1)
	{
		Close();
		perror("error listen");
	}
	return res;
}

//��ȡ����
int Socket::Accept(struct sockaddr_in& clientaddr)
{
	socklen_t lengthofsockaddr = sizeof(clientaddr);
	int clientfd = ::accept(fd_, (struct sockaddr*)&clientaddr, &lengthofsockaddr);
	if (clientfd < 0)
	{
		if (errno == EAGAIN)
		{
			return 0;
		}

		return clientfd;
	}
	return clientfd;
}
int Socket::Accept(struct sockaddr* addr, socklen_t* addrlen)
{
	int clientfd = ::accept(fd_, addr, addrlen);
	if (clientfd < 0)
	{
		perror("error accept");
		return clientfd;
	}
	return clientfd;
}
int Socket::Connect(const struct sockaddr* addr, socklen_t addrlen)
{
	if (::connect(fd_, addr, addrlen) == -1)
	{
		perror("error connect");
		return -1;
	}

	return 1;
}
ssize_t Socket::Send(const void* buf, size_t len, int flags)
{
	ssize_t ret = ::send(fd_, buf, len, flags);
	if (ret == -1) {
		perror("fail send data");
		return ret;
	}
	return ret;
}
ssize_t Socket::Recv(void* buf, size_t len, int flags)
{
	ssize_t ret = ::recv(fd_, buf, len, flags);
	if (ret == -1) {
		perror("fail recv data");
		return ret;
	}
	return ret;
}


// ���õ�ַ����
void Socket::SetReuseAddr()
{
	int on = 1;
	setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

void Socket::SetSockOpt(int level, int optname, const void* optval, socklen_t optlen)
{
	if (::setsockopt(fd_, level, optname, optval, optlen) == -1) {
		perror("setsockopt fail");
	}
}

void Socket::Close()
{
	::close(fd_);
	std::cout << "server socket Close..." << std::endl;
}

//�жϴ����׽����Ƿ�ɹ�
bool Socket::IsCreate()
{
	if (fd_ == -1)
	{
		return false;
	}

	return true;
}

//�����ǰ�׽���ip�Ͷ˿�
void Socket::PrintIPAndPort()
{
	sockaddr_storage storage;                       // �ܹ���Ӧ��ͬ����ĵ�ַЭ��ṹ
	socklen_t   sock_len = sizeof(storage);         // �������ֵ
	int ret = getsockname(fd_, (sockaddr*)&storage, &sock_len);        //����fd�õ���ַ��Ϣ
	if (ret < 0)
	{
		std::cout << "getsockname error :" << errno << std::endl;
		return;
	}

	if (storage.ss_family == AF_INET)
	{
		sockaddr_in* addr = (sockaddr_in*)&storage;
		std::cout << "local addr: " << inet_ntoa(addr->sin_addr) << ":" << ntohs(addr->sin_port) << std::endl;
	}
	else if (storage.ss_family == AF_INET6)
	{
		sockaddr_in6* addr = (sockaddr_in6*)&storage;
		char ip[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(addr));
		std::cout << "local addr: " << ip << ":" << ntohs(addr->sin6_port) << std::endl;

	}
}