/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: TCPSocket.hpp
 * @Author: zgscsed
 * @Date: 2023��5��10��23:01:45
 * @LastEditors: zgscsed
 * @LastEditTime: 2023��5��10��23:01:49
 * @Description: socket ����
	�� TCPSocket ���У�����ͨ���̳� Socket �ಢ����µķ��� sendto() �� recvfrom() ��ʵ�� UDP �ķ��ͺͽ��չ���
 */


#ifndef TCP_SOCKET_HPP_
#define TCP_SOCKET_HPP_

#include "net/Socket.hpp"

class TCPsocket : public Socket {
public:
    TCPsocket();
    virtual ~TCPsocket();

    // ����Ϊ������
    void SetNonblocking(bool nonblocking);
    void Shutdown(int how);

    void Sendall(const void* buf, size_t len, int flags);
    ssize_t Recv(void* buf, size_t len, int flags);
};

#endif // !TCP_SOCKET_HPP_
