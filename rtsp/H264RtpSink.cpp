/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: H264RtpSink.cpp
 * @Author: zgscsed
 * @Date: 2024��3��22��22:24:30
 * @LastEditors: zgscsed
 * @LastEditTime: 2024��3��22��22:24:30
 * @Description: H264RtpSink �� ����
    ����ƵԴ��֡���ݷ�װ��h264 rtp���� ���͡�
 */
#include "H264RtpSink.hpp"
#include <string.h>

H264RtpSink* H264RtpSink::Create(UsageEnvironment* env, MediaSource* mediaSource, int payloadType)
{
	H264RtpSink* rtpSink = new H264RtpSink(env, mediaSource, payloadType);
	return rtpSink;
}

H264RtpSink::H264RtpSink(UsageEnvironment* env, MediaSource* mediaSource, int payloadType)
	:RtpSink(env, mediaSource, payloadType),
	fps_(mediaSource->GetFps()), clockRate_(90000)
{
	Start(25);
}
H264RtpSink::~H264RtpSink()
{

}

void H264RtpSink::HandleFrame(AVFrame* frame)
{
    uint8_t naluType;        // nule��һ���ֽ�

    naluType = frame->frame_[0];
    RtpHeader* rtpHeader = &rtpPacket_->rtpHeader;

    if (frame->frameSize_ <= RTP_MAX_PKT_SIZE)      // nalu����С����������ʹ�õ�һnalu��Ԫģʽ
    {
        /*
        *   0 1 2 3 4 5 6 7 8 9
        *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *  |F|NRI|  Type   | a single NAL unit ... |
        *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        */
        memcpy(rtpPacket_->payload, frame->frame_, frame->frameSize_);
        SendRtpPacket(rtpPacket_, frame->frameSize_);

        seq++;
        if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8)  // ���SPS��PPS����Ҫ��ʱ���
        {
            return;
        }
    }
    else  // nalu ����С������������Ƭģʽ
    {
        /*
         *  0                   1                   2
         *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | FU indicator  |   FU header   |   FU payload   ...  |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

         /*
          *     FU Indicator
          *    0 1 2 3 4 5 6 7
          *   +-+-+-+-+-+-+-+-+
          *   |F|NRI|  Type   |
          *   +---------------+
          */

          /*
           *      FU Header
           *    0 1 2 3 4 5 6 7
           *   +-+-+-+-+-+-+-+-+
           *   |S|E|R|  Type   |
           *   +---------------+
           */
        int pktNum = frame->frameSize_ / RTP_MAX_PKT_SIZE;    // ������������
        int remainPktSize = frame->frameSize_ % RTP_MAX_PKT_SIZE;   // ���������Ĵ�С
        int i, pos = 1;

        // ���������İ�
        for (int i = 0; i < pktNum; ++i)
        {
            rtpPacket_->payload[0] = (naluType & 0x60) | 28;
            rtpPacket_->payload[1] = naluType & 0x1F;

            if (i == 0) // �װ�
            {
                rtpPacket_->payload[1] |= 0x80;       // start
            }
            else if (remainPktSize == 0 && i == pktNum - 1)  // ���һ����
            {
                rtpPacket_->payload[1] |= 0x40;       // end
            }

            memcpy(rtpPacket_->payload + 2, frame->frame_ + pos, RTP_MAX_PKT_SIZE);
            SendRtpPacket(rtpPacket_, RTP_MAX_PKT_SIZE + 2);

            seq++;
            pos += RTP_MAX_PKT_SIZE;
        }

        // ����ʣ������
        if (remainPktSize > 0)
        {
            rtpPacket_->payload[0] = (naluType & 0x60) | 28;
            rtpPacket_->payload[1] = naluType & 0x1F;
            rtpPacket_->payload[1] |= 0x40;    // end

            memcpy(rtpPacket_->payload + 2, frame + pos, remainPktSize + 2);
            SendRtpPacket(rtpPacket_, remainPktSize + 2);

            seq++;
        }
    }
    timestamp += clockRate_ / fps_;
}