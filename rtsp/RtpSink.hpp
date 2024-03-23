/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: RtpSink.hpp
 * @Author: zgscsed
 * @Date: 2024��3��20��22:27:22
 * @LastEditors: zgscsed
 * @LastEditTime: 2024��3��20��22:27:22
 * @Description: RtpSink �� ����
	����ƵԴ��֡���ݷ�װ��ָ��������rtp�������͡�
 */

#include "MediaSource.hpp"
#include "rtp.h"
#include "net/Timer.hpp"
#include <functional>

#ifndef RTP_SINK_HPP_
#define RTP_SINK_HPP_

class RtpSink
{
public:
	using SendPacketCallback = std::function<void(int traceId, RtpPacket*, int frameSize)>;
	
	virtual ~RtpSink();

	void SetSendPacketCallback(SendPacketCallback callback);
	void SetTraceId(const int traceId) { traceId_ = traceId; }
protected:
	RtpSink(UsageEnvironment* env, MediaSource* mediaSource, int payloadType);
	// ��frame������װ��rtp��,����
	virtual void HandleFrame(AVFrame* frame) = 0;

	// ����rtp����
	void SendRtpPacket(RtpPacket* rtpPacket, int frameSize);

	void Start(int ms);
	void Stop();
private:
	// ��ʱ����rtp����
	void TimeoutCallback();       
protected:
	// rtpHeader �Ĳ���
	uint8_t csrcLen;
	uint8_t extension;
	uint8_t padding;
	uint8_t version;
	uint8_t payloadType;;
	uint8_t marker;
	uint16_t seq;
	uint32_t timestamp;
	uint32_t ssrc;

	UsageEnvironment* env_;
	SendPacketCallback sendPacketCallback_;
	int traceId_;
	MediaSource* mediaSource_;

	Timer *timer_;

};
#endif // !RTP_SINK_HPP_

