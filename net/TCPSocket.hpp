/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: TCPSocket.hpp
 * @Author: zgscsed
 * @Date: 2023年5月10日23:01:45
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年5月10日23:01:49
 * @Description: socket 子类
	在 TCPSocket 类中，我们通过继承 Socket 类并添加新的方法 sendto() 和 recvfrom() 来实现 UDP 的发送和接收功能
 */


#ifndef TCP_SOCKET_HPP_
#define TCP_SOCKET_HPP_

#include "Socket.hpp"

class TCPSocket : public Socket {
public:
    TCPSocket();
    virtual ~TCPSocket();

    // 设置为非阻塞
    void SetNonblocking();
    void Shutdown(int how);

    void Sendall(const void* buf, size_t len, int flags);
    ssize_t Recv(void* buf, size_t len, int flags);
};

#endif // !TCP_SOCKET_HPP_
