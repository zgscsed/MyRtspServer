/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: MediaSession.hpp
 * @Author: zgscsed
 * @Date: 2024��3��23��20:30:34
 * @LastEditors: zgscsed
 * @LastEditTime: 2024��3��23��20:30:34
 * @Description: MediaSession �� ����rtsp�Ự��ý����
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
	// һ·��ƵԴ
	static MediaSession* Create(UsageEnvironment* env);
	~MediaSession();

	bool AddRtpSink(int traceId, RtpSink* rtpSink);
	void RemoveRtpSink(int traceId);

	bool AddRtpEndPoint(int traceId, RtpEndPoint* rtpEndPoint);
	bool RemoveRtpEndPoint(int traceId, RtpEndPoint* rtpEndPoint);

	bool AddRtspEndPoint(int traceId, RtspEndPoint* rtspEndPoint);
	bool RemoveRtspEndPoint(int traceId, RtspEndPoint* rtspEndPoint);

private:
	// trace������ָ������и����÷������ͷ���Դ
	class Trace
	{
	public:
		int traceId_;
		RtpSink* rtpSink_;
		bool isAlive_;
		std::list<RtpEndPoint*> rtpList_;
		std::list<RtspEndPoint*> rtspList_;

		// ��Ҫ����ý����Э�̵�socket��Ϣ�����ܽ�rtp����ȥ
	};
	explicit MediaSession(UsageEnvironment* env);

	void HandleSendRtpPacket(int traceId, RtpPacket* rtpPacket, int frameSize);
	void SendRtpPacket(int traceId, RtpPacket* rtpPacket, int frameSize);

	std::map<int, Trace*> traceMap_;
	UsageEnvironment* env_;

};

#endif // !MEDIA_SESSION_HPP_
