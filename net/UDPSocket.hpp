/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: UDPSocket.hpp
 * @Author: zgscsed
 * @Date: 2021-04-25 13:55:09
 * @LastEditors: zgscsed
 * @LastEditTime: 2023��4��17��22:50:56
 * @Description: socket ����
	�� UDPsocket ���У�����ͨ���̳� Socket �ಢ����µķ��� sendto() �� recvfrom() ��ʵ�� UDP �ķ��ͺͽ��չ���
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