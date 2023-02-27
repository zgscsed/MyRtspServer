/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: RtspParser.hpp
 * @Author: zgscsed
 * @Date: 2023-02-24 22:42:40
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-02-26 22:20:14
 * @Description: 对于RtspMessage结构体的操作封装
 */

#ifndef RTSP_PARSER_H
#define RTSP_PARSER_H
#include "struct.hpp"
#include <string>

class RtspParser {
public:

    RtspMessage* ParseMsg(std::string &msg);     // 将字符串解析成自定义结构

    void ParseRequest(RtspMessage* rtsp, std::string &msg, int pos);
    void ParseResponse(RtspMessage* rtsp, std::string &msg, int pos);

    // 判断请求消息还是响应
    bool IsRequest();
    // 解析请求行
    int ParseRequestLine(RtspMessage* rtsp, std::string &msg, int pos);
    // 解析请求头
    int ParseRequestHeader(RtspMessage* rtsp, std::string &msg, int pos);

    // 解析消息体
    int ParseRequestBody(RtspMessage* rtsp, std::string &msg, int pos);

private:
    // 字符串method 转成枚举值
    bool SetRtspType(RtspMessage* rtsp);
};


// 提供一些访问和修改RtspMessage对象的函数

// find header
bool FindHeader(std::list<RtspHeader> &headers, std::string key, RtspHeader &header);


#endif  //RTSP_PARSER_H