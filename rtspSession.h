/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp�������ĻỰ�����࣬ʵ�ֽ���rstp����

*/

#ifndef RTSPSESSION_H
#define RTSPSESSION_H

#include <iostream>
#include <map>
#include <string>

#include "udpSocket.h"

//Rtsp ������Ϣ�ṹ��
typedef struct _RtspRequestContext {
	std::string method;
	std::string url;
	std::string version;

	std::map<std::string, std::string> header;
	std::string body;
}RtspRequestContext;

//Rtsp ��Ӧ��Ϣ�ṹ��
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

	//������Ϣ
	bool praseRtspRequest(std::string&s, RtspRequestContext & rtspRequestContext);

	//������Ϣ
	void rtspProcess(const RtspRequestContext& rtspRequestContext, std::string&responseContext);


private:
	UdpSocket serverRtpFd_;                     //udp����rtp����
	UdpSocket serverRtcpFd_;                    //udp����rtcp����

	int clientRtpPort_;
	int clientRtcpPort_;

	int serverRtpPort_;
	int serverRtcpPort_;

};

#endif // !RTSPSESSION_H


