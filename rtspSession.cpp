/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp服务器的会话解析类，实现解析rstp交互

*/

#include "rtspSession.h"
#include "rtsp/RtspParser.hpp"
#include <sstream>
#include <cstring> 
RtspSession::RtspSession(int rtpPort, int rtcpPort)
	: serverRtpFd_(),
	serverRtcpFd_()
{
	serverRtpFd_.setReuseAddr();
	serverRtpFd_.bindAddr(rtpPort);

	serverRtcpFd_.setReuseAddr();
	serverRtcpFd_.bindAddr(rtcpPort);

	this->serverRtpPort_ = rtpPort;
	this->serverRtcpPort_ = rtcpPort;
}
RtspSession::~RtspSession()
{
	serverRtpFd_.close();
	serverRtcpFd_.close();
}

//解析消息
bool RtspSession::praseRtspRequest(std::string&msg, RtspRequestContext & rtspRequestContext)
{
/*
请求格式：
method url vesion\r\n        //请求行
CSeq: x\r\n                  //首部行, 也叫请求头
xxx\r\n
...
\r\n                         //首部行

//实体
*/

	RtspParser parser;
	parser.ParseMsg(msg);

	return true;
}

bool RtspSession::praseRtspRequest(std::string&msg, RtspMessage **rtspMessage)
{
	RtspParser parser;
	*rtspMessage = parser.ParseMsg(msg);
}

//处理消息
void RtspSession::rtspProcess(const RtspRequestContext& rtspRequestContext, std::string&responseContext)
{
	char result[100];
	std::string cseqs("Cseq");
	auto iter =  rtspRequestContext.header.find(cseqs);
	int cseq = atoi(iter->second.c_str());
	if (rtspRequestContext.method == "OPTIONS")
	{
		sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Public: OPTIONS, DESCRIBE, SETUP, PLAY\r\n"
			"\r\n",
			cseq);
	}
	else if (rtspRequestContext.method == "DESCRIBE")
	{
		char sdp[500];
		char localIp[100];

		sscanf(rtspRequestContext.url.c_str(), "rtsp://%[^:]:", localIp);

		sprintf(sdp, "v=0\r\n"
			"o=- 9%ld 1 IN IP4 %s\r\n"
			"t=0 0\r\n"
			"a=control:*\r\n"
			"m=video 0 RTP/AVP 96\r\n"
			"a=rtpmap:96 H264/90000\r\n"
			"a=control:track0\r\n",
			time(NULL), localIp);

		sprintf(result, "RTSP/1.0 200 OK\r\nCSeq: %d\r\n"
			"Content-Base: %s\r\n"
			"Content-type: application/sdp\r\n"
			"Content-length: %d\r\n\r\n"
			"%s",
			cseq,
			rtspRequestContext.url.c_str(),
			strlen(sdp),
			sdp);
	}
	else if (rtspRequestContext.method == "SETUP")
	{
		//先解析客户端rtp端口号
		auto it = rtspRequestContext.header.find("Transport");         //找到Transport
		if (it != rtspRequestContext.header.end())
		{
			//从字符串中，取出对应的值
			sscanf(it->second.c_str(), "RTP/AVP;unicast;client_port=%d-%d\r\n",
				&clientRtpPort_, &clientRtcpPort_);
		}

		sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Transport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d\r\n"
			"Session: 66334873\r\n"
			"\r\n",
			cseq,
			clientRtpPort_,
			clientRtcpPort_,
			serverRtpPort_,
			serverRtcpPort_);
	}
	else if(rtspRequestContext.method == "PLAY")
	{
		sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Range: npt=0.000-\r\n"
			"Session: 66334873; timeout=60\r\n\r\n",
			cseq);
	}

	//播放
	if (rtspRequestContext.method == "PLAY")
	{

	}

	responseContext = "";
	responseContext = std::string(result);
}

void RtspSession::rtspProcess(RtspMessage *rtspMessage, std::string&responseContext)
{
	char result[100];
	std::string cseqs("CSeq");
	RtspHeader header;
	int cseq = 0;
	bool isFind = FindHeader(rtspMessage->request->headers, cseqs, header);
	if (isFind)
	{
		cseq = atoi(header.value.c_str());
	}

	std::cout <<"type:" <<rtspMessage->rtspType<<std::endl;
	switch (rtspMessage->rtspType)
	{
	case RTSP_OPTIONS:
		sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Public: OPTIONS, DESCRIBE, SETUP, PLAY\r\n"
			"\r\n",
			cseq);
		break;
	case RTSP_DESCRIBE:
	{
		char sdp[500];
		char localIp[100];

		sscanf(rtspMessage->request->url.c_str(), "rtsp://%[^:]:", localIp);

		sprintf(sdp, "v=0\r\n"
			"o=- 9%ld 1 IN IP4 %s\r\n"
			"t=0 0\r\n"
			"a=control:*\r\n"
			"m=video 0 RTP/AVP 96\r\n"
			"a=rtpmap:96 H264/90000\r\n"
			"a=control:track0\r\n",
			time(NULL), localIp);

		sprintf(result, "RTSP/1.0 200 OK\r\nCSeq: %d\r\n"
			"Content-Base: %s\r\n"
			"Content-type: application/sdp\r\n"
			"Content-length: %d\r\n\r\n"
			"%s",
			cseq,
			rtspMessage->request->url.c_str(),
			strlen(sdp),
			sdp);
	}
		break;
	case RTSP_SETUP:
	{
		//先解析客户端rtp端口号
		bool isFind = FindHeader(rtspMessage->request->headers, "Transport", header);    //找到Transport
		if (isFind)
		{
			std::cout <<"value:"<< header.value<<std::endl;
			//从字符串中，取出对应的值
			sscanf(header.value.c_str(), "RTP/AVP;unicast;client_port=%d-%d\r\n",
				&clientRtpPort_, &clientRtcpPort_);
		}
		std::cout << "client rtp port:"<<clientRtpPort_<<std::endl;
		sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Transport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d\r\n"
			"Session: 66334873\r\n"
			"\r\n",
			cseq,
			clientRtpPort_,
			clientRtcpPort_,
			serverRtpPort_,
			serverRtcpPort_);
		break;
	}
	case RTSP_PLAY:
	{
		sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Range: npt=0.000-\r\n"
			"Session: 66334873; timeout=60\r\n\r\n",
			cseq);
		break;
	}
	case RTSP_TEARDOWN:
	{
		break;
	}
	default:
		break;
	}

	responseContext = "";
	responseContext = std::string(result);
}