/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: Socket.hpp
 * @Author: zgscsed
 * @Date: 2021-04-25 13:55:09
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年4月16日23:14:33
 * @Description: socket 基类
	在基类 Socket 中定义了一些常用的 socket 操作函数，
	包括 bind()、listen()、accept()、connect()、send()、recv() 和 setsockopt() 等。
	同时，在基类中定义了一个 fd_ 变量，用于保存 socket 描述符。
 */

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
class Socket {
public:
    Socket();
    Socket(int domain, int type, int protocol);
    virtual ~Socket();

    // 创建fd
    bool Create(int domain, int type, int protocol);
    int Bind(int port);
    int Bind(const struct sockaddr* addr, socklen_t addrlen);
    int Listen(int backlog);

    //获取连接
    int Accept(struct sockaddr_in& clientaddr);
    int Accept(struct sockaddr* addr, socklen_t* addrlen);
    int Connect(const struct sockaddr* addr, socklen_t addrlen);
    // flags 默认0 和 read 和 write函数效果一致
    ssize_t Send(const void* buf, size_t len, int flags);
    ssize_t Recv(void* buf, size_t len, int flags);

    // 设置地址复用
    void SetReuseAddr();
    void SetSockOpt(int level, int optname, const void* optval, socklen_t optlen);

    void Close();

    int GetFd() const { return fd_; }


    //判断创建套接字是否成功
    bool IsCreate();

    //输出当前套接字ip和端口
    void PrintIPAndPort();

protected:
    int fd_;

    int domain_;
    int type_;
    int protocol_;
};


#endif // SOCKET_HPP_
