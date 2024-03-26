/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: MediaSession.hpp
 * @Author: zgscsed
 * @Date: 2024年3月23日20:30:34
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月23日20:30:34
 * @Description: MediaSession 类 管理rtsp会话的媒体面
 */
#ifndef MEDIA_SESSION_HPP_
#define MEDIA_SESSION_HPP_

#include "net/UsageEnvironment.hpp"
#include "RtpSink.hpp"
#include "RtpEndPoint.hpp"

#include <map>
#include <list>
class MediaSession
{
public:
	// 一路视频源
	static MediaSession* Create(UsageEnvironment* env, std::string name);
	~MediaSession();

	bool AddRtpSink(int traceId, RtpSink* rtpSink);
	void RemoveRtpSink(int traceId);

	bool AddRtpEndPoint(int traceId, RtpEndPoint* rtpEndPoint);
	bool RemoveRtpEndPoint(int traceId, RtpEndPoint* rtpEndPoint);

	bool AddRtcpEndPoint(int traceId, RtcpEndPoint* rtspEndPoint);
	bool RemoveRtcpEndPoint(int traceId, RtcpEndPoint* rtspEndPoint);

	std::string GetMediaSessionName() { return name_; }

private:
	// trace对象中指针对象，有更调用方管理释放资源
	class Trace
	{
	public:
		int traceId_;
		RtpSink* rtpSink_;
		bool isAlive_;  // true: 可以发送媒体流
		std::list<RtpEndPoint*> rtpList_;
		std::list<RtcpEndPoint*> rtcpList_;

		// 需要保存媒体面协商的socket信息，才能将rtp发出去
	};
	explicit MediaSession(UsageEnvironment* env, std::string name);

	void HandleSendRtpPacket(int traceId, RtpPacket* rtpPacket, int frameSize);
	void SendRtpPacket(int traceId, RtpPacket* rtpPacket, int frameSize);

	std::string name_;

	std::map<int, Trace*> traceMap_;
	UsageEnvironment* env_;

};

#endif // !MEDIA_SESSION_HPP_
