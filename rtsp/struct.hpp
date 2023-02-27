/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: struct.hpp
 * @Author: zgscsed
 * @Date: 2023-02-24 22:18:25
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-02-26 19:34:26
 * @Description: rtsp 消息结构体
 */

#ifndef STRUCT_H
#define STRUCT_H
#include <string>
#include <list>

#define RTSP_SUCCESS 0
#define RTSP_ERROR -1

extern const std::string CRLF;
extern const std::string CRLFCRLF;

// 请求消息类型
enum RTSP_TYPE
{
	RTSP_OPTIONS,
	RTSP_DESCRIBE,
	RTSP_SETUP,
	RTSP_PLAY,
	RTSP_TEARDOWN,
	RTSP_BUTT
};


// 头域额外的参数
typedef struct _GenePara {
	std::string attr;    // 描述
	std::string value;   // 值
}GenePara;


// header
typedef struct _RtspHeader {
	std::string key;
	std::string value;

	std::list<GenePara> para;
}RtspHeader;

//Rtsp 请求消息结构体
struct RtspRequest {
	std::string method;
	std::string url;
	std::string version;

	std::list<RtspHeader> headers;
	std::string body;
};

//Rtsp 响应消息结构体
struct RtspResponse {
	std::string version;
	std::string statecode;
	std::string statemsg;

	std::list<RtspHeader> headers;
	std::string body;
};

// Rtsp 消息
struct RtspMessage {
	RtspMessage()
	{
		request = nullptr;
		response = nullptr;

		rtspType = RTSP_BUTT;
		isRequest = true;

		session = -1;
	}
	~RtspMessage()
	{
		if (request != nullptr)
		{
			delete request;
		}
		if (response != nullptr)
		{
			delete response;
		}
	}
	RtspRequest* request;
	RtspResponse* response;

	RTSP_TYPE rtspType;              // 请求类型
	bool isRequest;             // 当前是request还是response
	int session;                // 标识同一个会话
};

#endif //STRUCT_H