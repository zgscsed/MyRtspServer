/*
 * Copyright (C) 2022 zgscsed. All rights reserved.
 * @filename: logFile.hpp
 * @Author: zgscsed
 * @Date: 2022-03-21 21:50:23
 * @LastEditors: zgscsed
 * @LastEditTime: 2022-04-13 22:25:54
 * @Description: 同步写入日志
 */

#ifndef LOG_FILE_HPP_
#define LOG_FILE_HPP_

#include <cstdio>
namespace minlog{

class LogFile {
public:
    LogFile();
    ~LogFile();
    // 将日志写入缓存
    void Append(const char* data, size_t len);
    // 刷新
    void Flush();
private:



};



}

#endif // LOG_FILE_HPP_
