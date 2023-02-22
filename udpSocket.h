/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: file name
 * @Author: zgscsed
 * @Date: 2021-04-25 13:55:09
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-02-22 23:23:17
 * @Description: file content
 */
#ifndef UDPSOCKET_H
#define UDPSOCKET_H

/*
Copyright
time: 2021.4.25
author:zhoudong
desc: 服务器端socket类,udp传输，封装socket描述符和初始化操作

*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class UdpSocket {
public:
	UdpSocket();
	~UdpSocket();

	//设置地址重用
	void setReuseAddr();

	//绑定地址
	bool bindAddr(int port);

	//返回fd
	int getFd() const { return fd_; }

	//关闭fd
	bool close();

	//判断创建套接字是否成功
	bool isCreate();

	//输出当前套接字ip和端口
	void printIPAndPort();

private:
	int fd_;
};

#endif // !UDPSOCKET_H

