/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: UDPSocket.cpp
 * @Author: zgscsed
 * @Date: 2021-04-25 13:55:09
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年4月17日22:50:56
 * @Description: socket 子类
    在 UDPsocket 类中，我们通过继承 Socket 类并添加新的方法 Sendto() 和 Recvfrom() 来实现 UDP 的发送和接收功能
    SetBroadcast() 函数用于设置 UDP socket 是否启用了广播，
    SetMulticast() 函数用于设置 UDP socket 是否启用了组播，它们会将 UDP socket 加入到一个组播组中。
 */
#include <iostream>

#include "UDPSocket.hpp"


UDPSocket::UDPSocket() : Socket(AF_INET, SOCK_DGRAM, 0)
{

}
UDPSocket::~UDPSocket()
{

}

void UDPSocket::Sendto(const void* buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen)
{
    ssize_t ret = ::sendto(fd_, buf, len, flags, dest_addr, addrlen);
    if (ret == -1) {
        std::cout << "Failed to send data" << std::endl;
        return;
    }
}
ssize_t UDPSocket::Recvfrom(void* buf, size_t len, int flags, struct sockaddr* src_addr, socklen_t* addrlen)
{
    ssize_t ret = ::recvfrom(fd_, buf, len, flags, src_addr, addrlen);
    if (ret == -1) {
        std::cout << "Failed to receive data" << std::endl;
        return ret;
    }

    return ret;
}

// 用于设置 UDP socket 是否启用了广播
void UDPSocket::SetBroadcast(bool broadcast)
{
    int optval = broadcast ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
}

// 用于设置 UDP socket 是否启用了组播，它们会将 UDP socket 加入到一个组播组中
void UDPSocket::SetMulticast(const std::string& multicast_addr, const std::string& interface_addr)
{
    struct ip_mreqn imr;
    inet_pton(AF_INET, multicast_addr.c_str(), &imr.imr_multiaddr);
    inet_pton(AF_INET, interface_addr.c_str(), &imr.imr_address);
    imr.imr_ifindex = 0;
    if (setsockopt(fd_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr)) == -1)
    {
        std::cout << "Failed to set multicast membership" << std::endl;
    }
}