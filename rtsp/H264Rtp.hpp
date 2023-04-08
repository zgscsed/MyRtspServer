/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: H264Rtp.hpp
 * @Author: zgscsed
 * @Date: 2023-02-27 22:36:44
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-03-03 22:11:20
 * @Description: H264 文件 打包成rtp的相关操作
 */


#ifndef H264_RTP_HPP
#define H264_RTP_HPP

#include "rtp.h"

int startCode3(char *buf);
int startCode4(char *buf);

char* findNextStartCode(char *buf, int len);

int getFrameFromH264File(int fd, char* frame, int size);


// 发送H264 使用rtp
int RtpSendH264Frame(int socket, const char* ip, int16_t port, struct RtpPacket *rtpPacket, char *frame, uint32_t framesize);

#endif // H264_RTP_HPP