/*
 * Copyright (C) 2022 zgscsed. All rights reserved.
 * @filename: logger.cpp
 * @Author: zgscsed
 * @Date: 2022-03-23 21:33:45
 * @LastEditors: zgscsed
 * @LastEditTime: 2022-04-13 21:44:51
 * @Description: 将日志内容格式化，并且通过调佣回调函数保存数据
 */

#include "logger.hpp"
#include <thread>
#include <sstream>
#include <cstdio>
#include <time.h>

using namespace minlog;
const char *LevelString[Logger::Level::NUM_LEVEL] = {"DEBUG", "INFO", "WARN", "ERROR"};

void defaultOutput(const char* msg, int len)
{
  size_t n = fwrite(msg, 1, len, stdout);
  //FIXME check n
  (void)n;
}

void defaultFlush()
{
  fflush(stdout);
}

Logger::OutputFunc g_outputFunc = defaultOutput;
Logger::FlushFunc g_flushFunc = defaultFlush;

// 构造函数参数：文件名字， 行数， 日志等级
Logger::Logger(std::string basename, int line, Level level)
    : logging_(basename, line, level)
{

}
Logger::~Logger()
{
    logging_.Finish();
    std::string buf(logging_.Buffer());
    g_outputFunc(buf.c_str(), buf.size());
}

Logger::Logging::Logging(std::string basename, int line, Level level)
    : basename_(basename),
      line_(line),
      logLevel_(level),
      stream_()
{
    stream_ << FormatTime() <<" ";
    std::thread::id tid = std::this_thread::get_id();
    //threadId转换成整形值，所需头文件<sstream>
    // std::ostringstream  oss;
    // oss << tid;
    // std::string strtid = oss.str();
    stream_ << tid<<" ";
    stream_ << " ["<<LevelString[logLevel_]<<"] ";
}
Logger::Logging::~Logging()
{

}

void Logger::Logging::Finish()
{
    // 添加文件路径和行数
    // stream_ <<" " << basename_ << " : " << std::to_string(line_) << '\n';
    stream_ << '\n';
}
std::string Logger::Logging::Buffer()
{
    return stream_.str();
}
// 格式化时间输出
std::string Logger::Logging::FormatTime()
{

    time_t t = time(NULL);  // 获取当前日历时间
    struct  tm *tm_time = localtime(&t);
    char curtime[40] = {0};
    sprintf(curtime, "%4d%02d%02d %02d:%02d:%02d",
        tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
        tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
    
    std::string fmtTime(curtime);

    return fmtTime;
}