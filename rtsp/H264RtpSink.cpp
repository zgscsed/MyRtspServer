/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: H264RtpSink.cpp
 * @Author: zgscsed
 * @Date: 2024年3月22日22:24:30
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月22日22:24:30
 * @Description: H264RtpSink 类 基类
    将视频源的帧数据封装成h264 rtp包， 发送。
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
    uint8_t naluType;        // nule第一个字节

    naluType = frame->frame_[0];
    RtpHeader* rtpHeader = &rtpPacket_->rtpHeader;

    if (frame->frameSize_ <= RTP_MAX_PKT_SIZE)      // nalu长度小于最大包长，使用单一nalu单元模式
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
        if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8)  // 如果SPS，PPS不需要加时间戳
        {
            return;
        }
    }
    else  // nalu 长度小于最大包长，分片模式
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
        int pktNum = frame->frameSize_ / RTP_MAX_PKT_SIZE;    // 完整包的数量
        int remainPktSize = frame->frameSize_ % RTP_MAX_PKT_SIZE;   // 不完整包的大小
        int i, pos = 1;

        // 发送完整的包
        for (int i = 0; i < pktNum; ++i)
        {
            rtpPacket_->payload[0] = (naluType & 0x60) | 28;
            rtpPacket_->payload[1] = naluType & 0x1F;

            if (i == 0) // 首包
            {
                rtpPacket_->payload[1] |= 0x80;       // start
            }
            else if (remainPktSize == 0 && i == pktNum - 1)  // 最后一个包
            {
                rtpPacket_->payload[1] |= 0x40;       // end
            }

            memcpy(rtpPacket_->payload + 2, frame->frame_ + pos, RTP_MAX_PKT_SIZE);
            SendRtpPacket(rtpPacket_, RTP_MAX_PKT_SIZE + 2);

            seq++;
            pos += RTP_MAX_PKT_SIZE;
        }

        // 发送剩余数据
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