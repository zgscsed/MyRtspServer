/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: rtp.cpp
 * @Author: zgscsed
 * @Date: 2023-02-27 20:11:41
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-03-03 23:00:05
 * @Description: rtp 操作实现
 */
#include "rtp.h"
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void RtpHeaderInit(struct RtpPacket* rtpPacket, uint8_t csrcLen, uint8_t extension,
                   uint8_t padding, uint8_t version, uint8_t payloadType, uint8_t marker,
                   uint16_t seq, uint32_t timestamp, uint32_t ssrc)
{
    rtpPacket->rtpHeader.csrcLen = csrcLen;
    rtpPacket->rtpHeader.extension = extension;
    rtpPacket->rtpHeader.padding = padding;
    rtpPacket->rtpHeader.version = version;
    rtpPacket->rtpHeader.payloadType = payloadType;
    rtpPacket->rtpHeader.marker = marker;
    rtpPacket->rtpHeader.seq = seq;
    rtpPacket->rtpHeader.timestamp = timestamp;
    rtpPacket->rtpHeader.ssrc = ssrc;
}

int RtpSendPacket(int socket, const char* ip, int16_t port, struct RtpPacket* rtpPacket, uint32_t dataSize)
{
    struct sockaddr_in addr;
    int ret;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    std::cout << "req:"<<rtpPacket->rtpHeader.seq<<";timestamp:"<<rtpPacket->rtpHeader.timestamp << ";ssrc:" << rtpPacket->rtpHeader.ssrc << std::endl;
    rtpPacket->rtpHeader.seq = htons(rtpPacket->rtpHeader.seq);
    rtpPacket->rtpHeader.timestamp = htons(0x12222);
    rtpPacket->rtpHeader.ssrc = htons(0x22222);

    ret = sendto(socket, (void*)rtpPacket, dataSize + RTP_HEADER_SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));

    rtpPacket->rtpHeader.seq = ntohs(rtpPacket->rtpHeader.seq);
    rtpPacket->rtpHeader.timestamp = ntohl(rtpPacket->rtpHeader.timestamp);
    rtpPacket->rtpHeader.ssrc = ntohl(rtpPacket->rtpHeader.ssrc);
    std::cout << "2 req:" << rtpPacket->rtpHeader.seq << ";timestamp:" << rtpPacket->rtpHeader.timestamp << ";ssrc:" << rtpPacket->rtpHeader.ssrc << std::endl;

    return ret;
}