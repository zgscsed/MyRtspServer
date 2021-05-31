/*
Copyright
time: 2021.4.25
author:zhoudong
desc: 服务器端socket类,udp传输，封装socket描述符和初始化操作

*/

#include "udpSocket.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <cstring>

UdpSocket::UdpSocket()
{
	//创建套接字
	fd_ = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd_ < 0)
	{
		std::cout << "udpsocket error..." << std::endl;
	}
	else
		std::cout << "udpsocket create" << std::endl;

}
UdpSocket::~UdpSocket()
{
	close();
	std::cout << "~udpSocket()" << std::endl;
}

//设置地址重用
void UdpSocket::setReuseAddr()
{
	int on = 1;
	setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
}

//绑定地址
bool UdpSocket::bindAddr(int port)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (::bind(fd_, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0)
	{
		close();
		perror("error bind");
		exit(1);
	}
	std::cout << "udpsocket bindaddress..." << std::endl;

	return true;
}

//关闭fd
bool UdpSocket::close()
{
	::close(fd_);
	std::cout << "udpsocket fd close..." << std::endl;

	return true;
}

//判断创建套接字是否成功
bool UdpSocket::isCreate()
{
	if (fd_ == -1)
	{
		return false;
	}

	return true;
}

//输出当前套接字ip和端口
void UdpSocket::printIPAndPort()
{
	sockaddr_storage storage;                       // 能够适应不同种类的地址协议结构
	socklen_t   sock_len = sizeof(storage);         // 必须给初值
	int ret = getsockname(fd_, (sockaddr*)&storage, &sock_len);        //根据fd得到地址信息
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