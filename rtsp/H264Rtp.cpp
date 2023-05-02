/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: H264Rtp.cpp
 * @Author: zgscsed
 * @Date: 2023-02-27 22:37:08
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-03-03 23:40:53
 * @Description: H264 文件 打包成rtp的相关操作
 */
#include "H264Rtp.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int startCode3(char *buf)
{
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int startCode4(char *buf)
{
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

char* findNextStartCode(char *buf, int len)
{
    int i = 0;

    if (len < 3)
    {
        return nullptr;
    }

    for (i = 0; i < len - 3; ++i)
    {
        if (startCode3(buf) || startCode4(buf))
        {
            return buf;
        }

        ++buf;
    }

    if (startCode3(buf))
    {
        return buf;
    }

    return nullptr;
}



int getFrameFromH264File(int fd, char* frame, int size)
{
    int rSize, frameSize;
    char* nextStartCode;

    if (fd < 0)
    {
        return fd;
    }

    rSize = read(fd, frame, size);
    // h264 每帧开始 00 00 00 01  或者 00 00 01
    if (!startCode3(frame) && !startCode4(frame))
    {
        return -1;
    }

    nextStartCode = findNextStartCode(frame + 3, rSize -3);
    if (!nextStartCode)
    {
        return -1;
    }
    else
    {
        frameSize = (nextStartCode - frame);
        lseek(fd, frameSize - rSize, SEEK_CUR);
    }
}

// 发送H264 使用rtp
int RtpSendH264Frame(int socket, const char* ip, int16_t port, struct RtpPacket *rtpPacket, char *frame, uint32_t frameSize)
{
    uint8_t naluType;        // nule第一个字节
    int sendBytes = 0;
    int ret;

    naluType = frame[0];

    if (frameSize <= RTP_MAX_PKT_SIZE)      // nalu长度小于最大包长，使用单一nalu单元模式
    {
/*
*   0 1 2 3 4 5 6 7 8 9
*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  |F|NRI|  Type   | a single NAL unit ... |
*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
        memcpy(rtpPacket->payload, frame, frameSize);
        ret = RtpSendPacket(socket, ip, port, rtpPacket, frameSize);
        //std::cout << "RtpSendPacket ret:"<<ret<<std::endl;
        if (ret < 0)
        {
            return -1;
        }

        rtpPacket->rtpHeader.seq++;
        sendBytes += ret;

        if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8)  // 如果SPS，PPS不需要加时间戳
        {
            return sendBytes;
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
        int pktNum = frameSize / RTP_MAX_PKT_SIZE;    // 完整包的数量
        int remainPktSize = frameSize % RTP_MAX_PKT_SIZE;   // 不完整包的大小
        int i, pos = 1;

        // 发送完整的包
        for (int i = 0; i < pktNum; ++i)
        {
            rtpPacket->payload[0] = (naluType & 0x60) | 28;
            rtpPacket->payload[1] = naluType & 0x1F;

            if (i == 0) // 首包
            {
                rtpPacket->payload[1] |= 0x80;       // start
            }
            else if (remainPktSize == 0 && i == pktNum -1)  // 最后一个包
            {
                rtpPacket->payload[1] |= 0x40;       // end
            }

            memcpy(rtpPacket->payload + 2, frame + pos, RTP_MAX_PKT_SIZE);
            ret = RtpSendPacket(socket, ip, port, rtpPacket, RTP_MAX_PKT_SIZE + 2);
            //std::cout << "RtpSendPacket2 ret:"<<ret<<std::endl;
            if (ret < 0)
            {
                return -1;
            }

            rtpPacket->rtpHeader.seq++;
            sendBytes += ret;
            pos += RTP_MAX_PKT_SIZE;
        }

        // 发送剩余数据
        if (remainPktSize > 0)
        {
            rtpPacket->payload[0] = (naluType & 0x60) | 28;
            rtpPacket->payload[1] = naluType & 0x1F;
            rtpPacket->payload[1] |= 0x40;    // end

            memcpy(rtpPacket->payload + 2, frame + pos, remainPktSize+2);
            ret = RtpSendPacket(socket, ip, port, rtpPacket, remainPktSize + 2);
            //std::cout << "RtpSendPacket3 ret:"<<ret<<std::endl;
            if (ret < 0)
            {
                return -1;
            }

            rtpPacket->rtpHeader.seq++;
            sendBytes += ret;
        }
    }

    return sendBytes;
}
