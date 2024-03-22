/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: RtpSink.cpp
 * @Author: zgscsed
 * @Date: 2024��3��20��22:27:22
 * @LastEditors: zgscsed
 * @LastEditTime: 2024��3��20��22:27:22
 * @Description: RtpSink ��
	����ƵԴ��֡���ݷ�װ��ָ��������rtp�������͡�
 */
#include "base/global.hpp"
#include "RtpSink.hpp"
#include <netinet/in.h>

RtpSink::RtpSink(UsageEnvironment* env, MediaSource* mediaSource, int payloadType)
	:env_(env), mediaSource_(mediaSource), sendPacketCallback_(nullptr),
	csrcLen(0), extension(0), padding(0), version(RTP_VESION), payloadType(payloadType),
	marker(0), seq(0), timestamp(0), ssrc(0x88923423)
{
	timer_ = new Timer(25, Timer::TimerType::TIMER_PERIOD, std::bind(&RtpSink::TimeoutCallback, this));
}

RtpSink::~RtpSink()
{
	delete timer_;
}

// ����rtp����
void RtpSink::SendRtpPacket(RtpPacket* rtpPacket, int frameSize)
{
	// ����rtpHeader
	rtpPacket->rtpHeader.csrcLen = csrcLen;
	rtpPacket->rtpHeader.extension = extension;
	rtpPacket->rtpHeader.padding = padding;
	rtpPacket->rtpHeader.version = version;
	rtpPacket->rtpHeader.payloadType = payloadType;
	rtpPacket->rtpHeader.marker = marker;
	rtpPacket->rtpHeader.seq = htons(seq);
	rtpPacket->rtpHeader.timestamp = htonl(timestamp);
	rtpPacket->rtpHeader.ssrc = htonl(ssrc);
	frameSize += RTP_HEADER_SIZE;      // RtpPacket�ṹ�壬����headerռ12�ֽڣ������Ǻ��泤�ȣ���˷��ͳ��ȼ���header�ĳ���
	if (sendPacketCallback_ != nullptr)
	{
		sendPacketCallback_(rtpPacket, frameSize);
	}
}

// ��ʱ����rtp���ģ� ��Ҫһ����ʱ���ഥ��
void RtpSink::TimeoutCallback()
{
	AVFrame* frame = mediaSource_->GetFrame();
	if (frame == nullptr)
	{
		LOG_ERROR << "not get frame";
		return;
	}

	this->HandleFrame(frame);
	// frameʹ�ú�Ż�
	mediaSource_->PutFrame(frame);
}
void RtpSink::Start(int interval)
{
	timer_->timeout_ = interval;
	env_->TimerMgr()->AddTimer(timer_);
}
void RtpSink::Stop()
{
	env_->TimerMgr()->RemoveTimer(timer_);
}


