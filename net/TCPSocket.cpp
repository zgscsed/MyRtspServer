
#include "net/TCPSocket.hpp"

#include <fcntl.h>

#include <iostream>

TCPSocket::TCPSocket() : Socket(AF_INET, SOCK_STREAM, 0)
{

}

TCPSocket::~TCPSocket()
{

}

// 设置为非阻塞
void TCPSocket::SetNonblocking(bool nonblocking)
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

void TCPSocket::Shutdown(int how)
{
    if (::shutdown(fd_, how) == -1)
    {
        std::cout << "Failed to shutdown socket" << std::endl;
    }
}

void TCPSocket::Sendall(const void* buf, size_t len, int flags)
{
    ssize_t total_sent = 0;
    while (total_sent < len) {
        ssize_t ret = ::send(fd_, (char*)buf + total_sent, len - total_sent, flags);
        if (ret == -1) {
            std::cout << "Failed to send data" << std::endl;
            return;
        }
        total_sent += ret;
    }
}
ssize_t TCPSocket::Recv(void* buf, size_t len, int flags)
{
    ssize_t ret = ::recv(fd_, buf, len, flags);
    if (ret == -1)
    {
        std::cout << "Failed to receive data" << std::endl;
        return ret;
    }

    return ret;
}