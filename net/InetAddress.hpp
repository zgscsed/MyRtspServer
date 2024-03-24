/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: InetAddress.hpp
 * @Author: zgscsed
 * @Date: 2024��3��24��15:00:08
 * @LastEditors: zgscsed
 * @LastEditTime: 2024��3��24��15:00:08
 * @Description: InetAddress ��
	��װ�����ַ.
	sockaddr_in�ṹ�巽��ʹ�ã�������ֵ������ֱ��תΪsockaddr��
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