/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: struct.hpp
 * @Author: zgscsed
 * @Date: 2023-02-24 22:18:25
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-02-24 22:38:39
 * @Description: rtsp 消息结构体
 */

#include <string>
#include <list>

// 头域额外的参数
typedef struct _GenePara {
	std::string attr;    // 描述
	std::string value;   // 值
}GenePara;


// header
typedef struct _RtspHeader {
	std::string key;
	std::string value;
}RtspHeader;

//Rtsp 请求消息结构体
typedef struct _RtspRequest {
	std::string method;
	std::string url;
	std::string version;

	std::list<GenePara> para;
	std::string body;
}RtspRequest;

//Rtsp 响应消息结构体
typedef struct _RtspResponse {
	std::string version;
	std::string statecode;
	std::string statemsg;

	std::map<std::string, std::string> header;
	std::string body;
}RtspResponse;

// Rtsp 消息
typedef struct _RtspMessage {
	RtspRequest* request;
	RtspResponse* response;

	int session;          // 标识同一个会话

}RtspMessage;