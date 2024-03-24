/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: RtpEndPoint.cpp
 * @Author: zgscsed
 * @Date: 2024年3月24日14:25:26
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月24日14:25:26
 * @Description: RtpEndPoint 类 rtp流的节点，收发rtp流
 */

#include "RtpEndPoint.hpp"
#include "base/global.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

RtpEndPoint* RtpEndPoint::Create(int localPort, int peerPort, std::string peerIp, RtpType type)
{
	RtpEndPoint* endpoint = new RtpEndPoint(localPort, peerPort, peerIp, type);
	return endpoint;
}

RtpEndPoint::RtpEndPoint(int localPort, int peerPort, std::string peerIp, RtpType type)
	: localPort_(localPort), peerPort_(peerPort), peerIp_(peerIp), type_(type), rtpSocket_(nullptr), peerAddr_(peerPort, peerIp)
{
	// 构造函数中给rtpSocket_赋值，避免后续调用函数的地方需要判断指针是否有效
	switch (type_)
	{
	case RtpEndPoint::RtpType::RTP_OVER_UDP:
		rtpSocket_ = std::make_unique<UDPSocket>();
		break;
	case RtpEndPoint::RtpType::RTP_OVER_TCP:
		break;
	default:
		break;
	}
	LOG_INFO << "RtpEndPoint construct, localport: " << localPort_ << " peerPort: " << peerPort_ <<
		" peerIp: " << peerIp_ << " rtp type: " << static_cast<int>(type);
}
RtpEndPoint::~RtpEndPoint()
{
	if (rtpSocket_)
	{
		rtpSocket_->Close();
	}
}

// 初始化，主要是socket设置，tcp需要建链
int RtpEndPoint::Init()
{
	switch (type_)
	{
	case RtpEndPoint::RtpType::RTP_OVER_UDP:
		rtpSocket_ = std::make_unique<UDPSocket>();
		rtpSocket_->SetReuseAddr();
		rtpSocket_->Bind(localPort_);
		return YZ_SUCCESS;
	case RtpEndPoint::RtpType::RTP_OVER_TCP:
		return YZ_SUCCESS;
	default:
		LOG_ERROR << "RtpEndPoint init error, type is not valid!";
	}
	return YZ_ERROR;
}
// 收发流的函数
ssize_t RtpEndPoint::SendRtp(RtpPacket* rtpPacket, int len)
{
	switch (type_)
	{
	case RtpEndPoint::RtpType::RTP_OVER_UDP:
		{
			// dynamic_cast， 会进行类型检查，安全的，但是需要基类至少有一个虚函数
			UDPSocket* socket = dynamic_cast<UDPSocket*>(rtpSocket_.get());
			return socket->Sendto((void*)rtpPacket, len + RTP_HEADER_SIZE, 0, peerAddr_.GetAddr(), sizeof(struct sockaddr));
		}
	case RtpEndPoint::RtpType::RTP_OVER_TCP:
		{
			return 0;
		}
	default:
		break;
	}

	return 0;
}