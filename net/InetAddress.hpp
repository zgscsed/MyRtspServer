/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: InetAddress.hpp
 * @Author: zgscsed
 * @Date: 2024年3月24日15:00:08
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月24日15:00:08
 * @Description: InetAddress 类
	封装网络地址.
	sockaddr_in结构体方便使用，便于设值，可以直接转为sockaddr。
 */

#ifndef INET_ADDRESS_HPP_
#define INET_ADDRESS_HPP_

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Ipv4Address
{
public:
	Ipv4Address();
	Ipv4Address(int port, std::string ip);
	~Ipv4Address();

	void SetAddress(int port, std::string ip);
	int GetPort() const;
	std::string GetIp() const;
	struct sockaddr* GetAddr() const;
private:
	std::string ip_;
	int port_;
	struct sockaddr_in addr_;
};

#endif // !INET_ADDRESS_HPP_