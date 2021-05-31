/*
Copyright
time: 2021.4.25
author:zhoudong
desc: ��������socket��,udp���䣬��װsocket�������ͳ�ʼ������

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
	//�����׽���
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

//���õ�ַ����
void UdpSocket::setReuseAddr()
{
	int on = 1;
	setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
}

//�󶨵�ַ
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

//�ر�fd
bool UdpSocket::close()
{
	::close(fd_);
	std::cout << "udpsocket fd close..." << std::endl;

	return true;
}

//�жϴ����׽����Ƿ�ɹ�
bool UdpSocket::isCreate()
{
	if (fd_ == -1)
	{
		return false;
	}

	return true;
}

//�����ǰ�׽���ip�Ͷ˿�
void UdpSocket::printIPAndPort()
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