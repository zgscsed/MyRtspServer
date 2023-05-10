/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: UDPSocket.hpp
 * @Author: zgscsed
 * @Date: 2021-04-25 13:55:09
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年4月17日22:50:56
 * @Description: socket 子类
	在 UDPsocket 类中，我们通过继承 Socket 类并添加新的方法 sendto() 和 recvfrom() 来实现 UDP 的发送和接收功能
 */
#ifndef UDP_SOCKET_HPP_
#define UDP_SOCKET_HPP_

#include "Socket.hpp"
class UDPSocket : public Socket {
public:
    UDPSocket();
    virtual ~UDPSocket();

    void Sendto(const void* buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen);
    ssize_t Recvfrom(void* buf, size_t len, int flags, struct sockaddr* src_addr, socklen_t* addrlen);

    // 用于设置 UDP socket 是否启用了广播
    void SetBroadcast(bool broadcast);
    // 用于设置 UDP socket 是否启用了组播，它们会将 UDP socket 加入到一个组播组中
    void SetMulticast(const std::string& multicast_addr, const std::string& interface_addr);
};

#endif //UDP_SOCKET_HPP_