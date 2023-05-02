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

    void sendto(const void* buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen);
    ssize_t recvfrom(void* buf, size_t len, int flags, struct sockaddr* src_addr, socklen_t* addrlen);
};

#endif //UDP_SOCKET_HPP_