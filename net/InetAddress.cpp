/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: InetAddress.cpp
 * @Author: zgscsed
 * @Date: 2024��3��24��15:00:08
 * @LastEditors: zgscsed
 * @LastEditTime: 2024��3��24��15:00:08
 * @Description: InetAddress ��
	��װ�����ַ.
	sockaddr_in�ṹ�巽��ʹ�ã�������ֵ������ֱ��תΪsockaddr��
 */


#include "InetAddress.hpp"

Ipv4Address::Ipv4Address()
{

}
Ipv4Address::Ipv4Address(int port, std::string ip)
	:port_(port), ip_(ip)
{
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

Ipv4Address::~Ipv4Address()
{

}

void Ipv4Address::SetAddress(int port, std::string ip)
{
	port_ = port;
	ip_ = ip;
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}
int Ipv4Address::GetPort() const
{
	return port_;
}
std::string Ipv4Address::GetIp() const
{
	return ip_;
}
struct sockaddr* Ipv4Address::GetAddr() const
{
	return (struct sockaddr*)(&addr_);
}