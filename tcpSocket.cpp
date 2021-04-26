/*
Copyright
time: 2021.4.24
author:zhoudong
desc: 服务器端socket类，封装socket描述符和初始化操作

*/
#include "tcpSocket.h"

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <cstring>
TcpSocket::TcpSocket()
{
	//创建套接字
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ == -1)
	{
		std::cout << "socket error" << std::endl;
	}
	else
		std::cout << "server socket create " << fd_ << std::endl;

}
TcpSocket::~TcpSocket()
{
	close();
	std::cout << "server socket close..." << std::endl;
}

//设置地址重用
void TcpSocket::setReuseAddr()
{
	int on = 1;
	setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

//设置非阻塞
void TcpSocket::setNonBlocking()
{
	int opts = fcntl(fd_, F_GETFL);              //获得fd文件的状态标志参数
	if (opts < 0)
	{
		std::cout << "fcntl(fd_, F_GETFL) error" << std::endl;
		exit(1);
	}

	if (fcntl(fd_, F_SETFL, opts | O_NONBLOCK) < 0)          //设置非阻塞
	{
		std::cout << "fcntl(fd_,F_SETFL, opts" << std::endl;
		exit(1);
	}
	std::cout << "server setnonblocking..." << std::endl;
}

//地址绑定
bool TcpSocket::bindAddr(int serverport)
{
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(serverport);

	int res = bind(fd_, (struct  sockaddr*)&serveraddr, sizeof(serveraddr));
	if (res == -1)
	{
		close();
		perror("error bind");
		exit(1);
	}
	std::cout << "server bindaddress..." << std::endl;

	return true;
}

//监听
bool TcpSocket::listen()
{
	if (::listen(fd_, 8888) < 0)
	{
		perror("error listen");
		close();
		exit(1);
	}
	std::cout << "server listenning..." << std::endl;
	return true;
}

//获取连接
int TcpSocket::accept(struct sockaddr_in &clientaddr)
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

//关闭fd
bool TcpSocket::close()
{
	::close(fd_);
	std::cout << "server socket close..." << std::endl;

	return true;
}

//判断创建套接字是否成功
bool TcpSocket::isCreate()
{
	if (fd_ == -1)
	{
		return false;
	}

	return true;
}