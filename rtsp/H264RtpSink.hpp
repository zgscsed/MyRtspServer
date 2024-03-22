/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: H264RtpSink.hpp
 * @Author: zgscsed
 * @Date: 2024��3��22��22:24:30
 * @LastEditors: zgscsed
 * @LastEditTime: 2024��3��22��22:24:30
 * @Description: H264RtpSink �� ����
	����ƵԴ��֡���ݷ�װ��h264 rtp���� ���͡�
 */

#ifndef H264_RTP_SINK_HPP_
#define H264_RTP_SINK_HPP_
#include "RtpSink.hpp"

class H264RtpSink : public RtpSink
{
public:
	static H264RtpSink* Create(UsageEnvironment* env, MediaSource* mediaSource, int payloadType = RTP_PAYLOAD_TYPE_H264);
	virtual ~H264RtpSink();

	virtual void HandleFrame(AVFrame* frame);
private:

	H264RtpSink(UsageEnvironment* env, MediaSource* mediaSource, int payloadType);

	RtpPacket* rtpPacket_;
	int fps_;
	// h264������Ĭ�� 90000 Hz��ʱ������
	int clockRate_;
};


#endif // !H264_RTP_SINK_HPP_
