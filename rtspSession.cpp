/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp服务器的会话解析类，实现解析rstp交互

*/

#include "rtspSession.h"
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
	std::string crlf("\r\n");           //请求消息的分隔符
	std::string crlfcrlf("\r\n\r\n");

	int prev = 0;
	int next = 0;

	if ((next = msg.find(crlf, prev)) != msg.npos)
	{
		//找到crlf出现的位置,并取出请求行消息
		std::string request_line(msg.substr(prev, next - prev));
		std::stringstream iss(request_line);
		iss >> rtspRequestContext.method;
		iss >> rtspRequestContext.url;
		iss >> rtspRequestContext.version;

		prev = next;
	}
	else
	{
		std::cout << "rtsp request request_line error" << std::endl;
		return false;
	}

	//解析请求头
	std::string key, value;
	int pos_crlfcrlf = 0;
	if ((pos_crlfcrlf = msg.find(crlfcrlf, prev)) != msg.npos)       //确定crlfcrlf的位置
	{
		while (prev != pos_crlfcrlf)        //最新的位置没有指导CRLFCRLF，说明中间有请求头数据
		{
			//找到第一组
			int pos = msg.find(crlf, prev + 2);      //prev+2原因是prev指向crlf， 一个有两个字符，需要跳过
			int pos_colon = msg.find(":", prev + 2);    //确定冒号的位置
			key = msg.substr(prev + 2, pos_colon - prev - 2);
			value = msg.substr(pos_colon + 2, pos - pos_colon - 2);    //冒号后有空格

			rtspRequestContext.header.insert(std::pair<std::string, std::string>(key, value));

			prev = pos;

		}
	}
	else
	{
		std::cout << "rtsp request request header error" << std::endl;
		return false;
	}

	//解析实体
	rtspRequestContext.body = msg.substr(pos_crlfcrlf + 4);

	msg.clear();

	return true;
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
			sscanf(it->second.c_str(), "Transport: RTP/AVP;unicast;client_port=%d-%d\r\n",
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