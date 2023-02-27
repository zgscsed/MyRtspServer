/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: H264Rtp.cpp
 * @Author: zgscsed
 * @Date: 2023-02-27 22:37:08
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-02-27 22:55:50
 * @Description: H264 文件 打包成rtp的相关操作
 */
#include "H264Rtp.hpp"

#include <cstdio>
#include <string>
#include <unistd.h>

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