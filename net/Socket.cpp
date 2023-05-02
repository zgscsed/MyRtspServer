#include "Socket.hpp"
#include <iostream>

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <cstring>

Socket::Socket(int domain, int type, int protocol)
{
	//创建套接字
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


// 设置地址复用
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