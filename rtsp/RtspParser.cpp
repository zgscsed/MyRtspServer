/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: RtspParser.cpp
 * @Author: zgscsed
 * @Date: 2023-02-25 21:18:20
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-02-26 22:21:25
 * @Description: 函数实现
 */

#include "RtspParser.hpp"

#include "../Tool.hpp"

#include <iostream>
#include <sstream>

// 将字符串解析成自定义结构
RtspMessage* RtspParser::ParseMsg(std::string &msg)
{
    RtspMessage *message = new RtspMessage();

    auto headerPos = ParseRequestLine(message, msg, 0);


    ParseRequest(message, msg, headerPos);

    return message;
}

void RtspParser::ParseRequest(RtspMessage* rtsp, std::string &msg, int pos)
{
    // 解析请求头
    int bodyPos = ParseRequestHeader(rtsp, msg, pos);
    if (bodyPos == RTSP_ERROR)
    {
        return;
    }
    //解析消息体
    ParseRequestBody(rtsp, msg, bodyPos);
}
void RtspParser::ParseResponse(RtspMessage* rtsp, std::string &msg, int pos)
{

}

// 判断请求消息还是响应
bool RtspParser::IsRequest()
{

}
// 解析请求行
int RtspParser::ParseRequestLine(RtspMessage* rtsp, std::string &msg, int pos)
{
    int prev = pos;
    int next = 0;

    if ((next = msg.find(CRLF, prev)) != msg.npos)
    {
        //找到crlf出现的位置,并取出请求行消息
        std::string request_line(msg.substr(prev, next - prev));
        std::stringstream iss(request_line);
        std::string method;
        iss >> method;

        if (method.find("RTSP") != method.npos)
        {
            // 设置响应
            rtsp->isRequest = false;
            rtsp->response = new RtspResponse();
            rtsp->response->version = method;
            iss >> rtsp->response->statecode;
            iss >> rtsp->response->statemsg;
        }
        else
        {
            // 设置请求
            rtsp->isRequest = true;
            rtsp->request = new RtspRequest();
            rtsp->request->method = method;
            iss >> rtsp->request->url;
            iss >> rtsp->request->version;

            // 设置请求类型
            SetRtspType(rtsp);
        }

        prev = next;
    }
    else
    {
        std::cout << "rtsp request request_line error" << std::endl;
        return RTSP_ERROR;
    }

    return next + 2;                    // 返回请求头起始位置
}
// 解析请求头
int RtspParser::ParseRequestHeader(RtspMessage* rtsp, std::string &msg, int pos)
{
    std::string key, value;
    int prev = pos;
    int pos_crlfcrlf = 0;
    std::cout <<"pos:"<<pos<<std::endl;
    if ((pos_crlfcrlf = msg.find(CRLFCRLF, prev)) != msg.npos)       // 确定crlfcrlf的位置
    {
        std::cout << "endpos:"<<pos_crlfcrlf<<std::endl;
        while (prev != pos_crlfcrlf && prev != pos_crlfcrlf + 2)                                // 最新的位置不是CRLFCRLF，说明中间有请求头数据
        {
            //找到第一组
            int nextpos = msg.find(CRLF, prev);
            int pos_colon = msg.find(":", prev);    //确定冒号的位置
            key = msg.substr(prev, pos_colon - prev);
            value = msg.substr(pos_colon + 1, nextpos - pos_colon - 1);    // 跳过冒号

            TinyTool::trim(value);       // 消除前后空格
            std::cout <<"nextpos:"<< nextpos << ",key:" << key << ",value:" << value << "" << std::endl;

            RtspHeader header;
            header.key = key;
            header.value = value;

            rtsp->request->headers.push_back(header);

            prev = nextpos + 2;          // \r\n 两个字符
        }
    }
    else
    {
        std::cout << "rtsp request request header error" << std::endl;
        return RTSP_ERROR;
    }
    std::cout << "end parse header" << std::endl;
    return pos_crlfcrlf + 4;   // 返回消息体起始位置

}

// 解析消息体
int RtspParser::ParseRequestBody(RtspMessage* rtsp, std::string &msg, int pos)
{
    if (pos < 0 && pos >= msg.size() && pos == std::string::npos)
    {
        std::cout << "ParseRequestBody pos is error"<<std::endl;
        return RTSP_ERROR;
    }
    // 解析消息体
	rtsp->request->body = msg.substr(pos);

    return RTSP_SUCCESS;
}

// 字符串method 转成枚举值
bool RtspParser::SetRtspType(RtspMessage* rtsp)
{
    std::string method = rtsp->request->method;
    if (method.compare("OPTIONS") == 0)
    {
        rtsp->rtspType = RTSP_OPTIONS;
        return true;
    }
    else if (method.compare("DESCRIBE") == 0)
    {
        rtsp->rtspType = RTSP_DESCRIBE;
        return true;
    }
    else if (method.compare("SETUP") == 0)
    {
        rtsp->rtspType = RTSP_OPTIONS;
        return true;
    }
    else if (method.compare("PLAY") == 0)
    {
        rtsp->rtspType = RTSP_SETUP;
        return true;
    }
    else if (method.compare("TEARDOWN") == 0)
    {
        rtsp->rtspType = RTSP_TEARDOWN;
        return true;
    }
    else
    {
        rtsp->rtspType = RTSP_BUTT;
        return true;
    }
    return true;
}


// find header
bool FindHeader(std::list<RtspHeader> &headers, std::string key, RtspHeader &header)
{
    for (auto &it : headers)
    {
        if (it.key == key)
        {
            header = it;
            return true;
        }
    }

    return false;
}