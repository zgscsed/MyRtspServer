/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: file name
 * @Author: zgscsed
 * @Date: 2021-04-26 21:57:48
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-02-26 22:34:47
 * @Description: file content
 */
/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp服务器的会话解析类，实现解析rstp交互

*/

#ifndef RTSPSESSION_H
#define RTSPSESSION_H

#include <iostream>
#include <map>
#include <list>
#include <string>

#include "rtsp/RtspParser.hpp"
#include "udpSocket.h"

//Rtsp 请求消息结构体
typedef struct _RtspRequestContext {
	std::string method;
	std::string url;
	std::string version;

	std::map<std::string, std::string> header;
	std::string body;
}RtspRequestContext;

//Rtsp 响应消息结构体
typedef struct _RtspResponseContext {
	std::string version;
	std::string statecode;
	std::string statemsg;

	std::map<std::string, std::string> header;
	std::string body;
}RtspResponseContext;

class RtspSession {
public:
	RtspSession(int rtpPort, int rtcpPort);
	~RtspSession();

	//解析消息
	bool praseRtspRequest(std::string&s, RtspRequestContext & rtspRequestContext);
	bool praseRtspRequest(std::string&s, RtspMessage **rtspMessage);

	//处理消息
	void rtspProcess(const RtspRequestContext& rtspRequestContext, std::string&responseContext);
	void rtspProcess(RtspMessage *rtspMessage, std::string&responseContext);


private:
	UdpSocket serverRtpFd_;                     //udp传输rtp数据
	UdpSocket serverRtcpFd_;                    //udp传输rtcp数据

	int clientRtpPort_;
	int clientRtcpPort_;

	int serverRtpPort_;
	int serverRtcpPort_;

};

#endif // !RTSPSESSION_H


