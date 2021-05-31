/*
Copyright
time: 2021.4.24
author:zhoudong
desc: ��������socket�࣬��װsocket�������ͳ�ʼ������

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
	//�����׽���
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

//���õ�ַ����
void TcpSocket::setReuseAddr()
{
	int on = 1;
	setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

//���÷�����
void TcpSocket::setNonBlocking()
{
	int opts = fcntl(fd_, F_GETFL);              //���fd�ļ���״̬��־����
	if (opts < 0)
	{
		std::cout << "fcntl(fd_, F_GETFL) error" << std::endl;
		exit(1);
	}

	if (fcntl(fd_, F_SETFL, opts | O_NONBLOCK) < 0)          //���÷�����
	{
		std::cout << "fcntl(fd_,F_SETFL, opts" << std::endl;
		exit(1);
	}
	std::cout << "server setnonblocking..." << std::endl;
}

//��ַ��
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

//����
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

//��ȡ����
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

//�ر�fd
bool TcpSocket::close()
{
	::close(fd_);
	std::cout << "server socket close..." << std::endl;

	return true;
}

//�жϴ����׽����Ƿ�ɹ�
bool TcpSocket::isCreate()
{
	if (fd_ == -1)
	{
		return false;
	}

	return true;
}

//�����ǰ�׽���ip�Ͷ˿�
void TcpSocket::printIPAndPort()
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