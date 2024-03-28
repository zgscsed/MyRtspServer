/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: RtpEndPoint.hpp
 * @Author: zgscsed
 * @Date: 2024年3月24日14:25:26
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月24日14:25:26
 * @Description: RtpEndPoint 类 rtp流的节点，收发rtp流.目前先实现发送流，接受流后续在实现
 */
#ifndef RTP_END_POINT_HPP_
#define RTP_END_POINT_HPP_

#include "rtp.h"
#include "net/UDPSocket.hpp"
#include "net/InetAddress.hpp"
#include <string>
#include <memory>
class RtpEndPoint
{
public:
	enum class RtpType
	{
		RTP_OVER_UDP,
		RTP_OVER_TCP
	};
	static RtpEndPoint* Create(int localPort, int peerPort, std::string peerIp, RtpType type);
	RtpEndPoint(int localPort, int peerPort, std::string peerIp, RtpType type);
	~RtpEndPoint();

	// 初始化，主要是socket设置，tcp需要建链
	int Init();
	// 收发流的函数
	ssize_t SendRtp(RtpPacket* rtpPacket, int len);

	int GetPeerPort() const { return peerPort_; }

	bool IsAlive() const { return  isAlive_; }
	void SetAlive(bool alive) { isAlive_ = alive; }

private:

	bool isAlive_;

	RtpType type_;
	int localPort_;
	int peerPort_;
	std::string peerIp_;      // 后续会将这个address封装成对象，提前格式化为网络序地址
	
	Ipv4Address peerAddr_;

	std::unique_ptr<Socket> rtpSocket_;
};

class RtcpEndPoint
{
public:
	static RtcpEndPoint* Create(int localPort, int peerPort, std::string peerIp, RtpEndPoint::RtpType type);
	~RtcpEndPoint();

	// 初始化，主要是socket设置，tcp需要建链
	int Init();
	// 收发流的函数
	ssize_t SendRtp(RtpPacket* rtpPacket, int len);

	int GetPeerPort() const { return rtpPoint_.GetPeerPort(); }
	bool IsAlive() const { return  rtpPoint_.IsAlive(); }
	void SetAlive(bool alive) { rtpPoint_.SetAlive(alive); }
private:
	RtcpEndPoint(int localPort, int peerPort, std::string peerIp, RtpEndPoint::RtpType type);
	RtpEndPoint rtpPoint_;
};

#endif // !RTP_END_POINT_HPP_