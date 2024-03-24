/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: MediaSession.hpp
 * @Author: zgscsed
 * @Date: 2024年3月23日20:30:34
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月23日20:30:34
 * @Description: MediaSession 类 管理rtsp会话的媒体面
 */
#include "MediaSession.hpp"
#include "base/global.hpp"

const int MAX_TRACE_NUM = 3;

MediaSession* MediaSession::Create(UsageEnvironment* env)
{
	MediaSession* session = new MediaSession(env);
	return session;
}
MediaSession::MediaSession(UsageEnvironment* env):
	env_(env)
{
	LOG_INFO << "MediaSession construct";
}
MediaSession::~MediaSession()
{

}

bool MediaSession::AddRtpSink(int traceId, RtpSink* rtpSink)
{
	if (rtpSink == nullptr)
	{
		LOG_ERROR << "AddRtpSink rtpSink is nullptr";
		return false;
	}
	if (traceId < 0 && traceId > MAX_TRACE_NUM)
	{
		LOG_ERROR << "AddRtpSink traceId is not valid, traceId: "<< traceId;
		return false;
	}
	Trace* trace = new Trace();
	trace->traceId_ = traceId;
	trace->rtpSink_ = rtpSink;
	trace->isAlive_ = true;
	rtpSink->SetTraceId(traceId);
	rtpSink->SetSendPacketCallback(std::bind(&MediaSession::HandleSendRtpPacket, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	traceMap_.insert(std::pair<int, Trace*>(traceId, trace));

	return true;
}

void MediaSession::RemoveRtpSink(int traceId)
{
	auto iter = traceMap_.find(traceId);
	if (iter == traceMap_.end())
	{
		LOG_ERROR << "RemoveRtpSink traceId: " << traceId << " is not find!";
		return;
	}
	traceMap_.erase(iter);
}

bool MediaSession::AddRtpEndPoint(int traceId, RtpEndPoint* rtpEndPoint)
{
	if (rtpEndPoint == nullptr)
	{
		LOG_ERROR << "AddRtpEndPoint rtpEndPoint is nullptr";
		return false;
	}
	if (traceId < 0 && traceId > MAX_TRACE_NUM)
	{
		LOG_ERROR << "AddRtpEndPoint traceId is not valid, traceId: " << traceId;
		return false;
	}
	auto iter = traceMap_.find(traceId);
	if (iter == traceMap_.end())
	{
		LOG_ERROR << "AddRtpEndPoint traceId: " << traceId << " is not find!";
		return false;
	}
	iter->second->rtpList_.push_back(rtpEndPoint);
	return true;

}
bool MediaSession::RemoveRtpEndPoint(int traceId, RtpEndPoint* rtpEndPoint)
{
	// Todo: 检测指针，可以用宏来实现，减少代码
	if (rtpEndPoint == nullptr)
	{
		LOG_ERROR << "RemoveRtpEndPoint rtpEndPoint is nullptr";
		return false;
	}
	if (traceId < 0 && traceId > MAX_TRACE_NUM)
	{
		LOG_ERROR << "RemoveRtpEndPoint traceId is not valid, traceId: " << traceId;
		return false;
	}
	auto iter = traceMap_.find(traceId);
	if (iter == traceMap_.end())
	{
		LOG_ERROR << "RemoveRtpEndPoint traceId: " << traceId << " is not find!";
		return false;
	}
	auto& lists = iter->second->rtpList_;
	for (auto it = lists.begin(); it != lists.end(); ++it)
	{
		if (*it == rtpEndPoint)
		{
			lists.erase(it);
			return true;
		}
	}
	LOG_ERROR << "RemoveRtpEndPoint rtpEndPoint peer port:"<< rtpEndPoint->GetPeerPort() <<" is not find!";
	return false;
}

void MediaSession::HandleSendRtpPacket(int traceId, RtpPacket* rtpPacket, int frameSize)
{
	SendRtpPacket(traceId, rtpPacket, frameSize);
}
void MediaSession::SendRtpPacket(int traceId, RtpPacket* rtpPacket, int frameSize)
{
	auto iter = traceMap_.find(traceId);
	if (iter == traceMap_.end())
	{
		LOG_ERROR << "SendRtpPacket traceId: " << traceId << " is not find!";
		return;
	}
	Trace* trace = iter->second;
	if (!trace->isAlive_)
	{
		return;
	}

	// 发送rtp, 缺少rtp实例列
}