/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: RtpSink.hpp
 * @Author: zgscsed
 * @Date: 2024年3月20日22:27:22
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月20日22:27:22
 * @Description: RtpSink 类 基类
	将视频源的帧数据封装成指定的类型rtp包，发送。
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
	// 将frame数据组装成rtp包,发送
	virtual void HandleFrame(AVFrame* frame) = 0;

	// 发送rtp报文
	void SendRtpPacket(RtpPacket* rtpPacket, int frameSize);

	void Start(int ms);
	void Stop();
private:
	// 定时发送rtp报文
	void TimeoutCallback();       
protected:
	// rtpHeader 的参数
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

