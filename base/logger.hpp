/*
 * Copyright (C) 2022 zgscsed. All rights reserved.
 * @filename: logger.hpp
 * @Author: zgscsed
 * @Date: 2022-03-21 21:43:57
 * @LastEditors: zgscsed
 * @LastEditTime: 2022-04-13 20:37:37
 * @Description: 将日志格式化，并写入指定的对象中。
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <string>
#include <sstream>

namespace minlog{

/*

*/
class Logger {
public:
    typedef void (*OutputFunc)(const char* msg, int len);
    typedef void (*FlushFunc)();
    // 暂时设置这个几个等级的日志
    enum Level{
        DEBUG,
        INFO,
        WARN,
        ERROR,
        NUM_LEVEL
    };

    // 构造函数参数：文件名字， 行数， 日志等级
    Logger(std::string basename, int line, Level level);
    ~Logger();

    void SetOutputFunc(OutputFunc);
    void SetFlushFunc(FlushFunc);

    int LogLevel(){ return logging_.logLevel_;}
    std::stringstream& Stream(){ return logging_.stream_;}
private:
// 内部类生成，时间，线程，文件等的日志
class Logging {
public:
    Logging(std::string basename, int line, Level level = Level::INFO);
    ~Logging();

    void Finish();
    std::string Buffer();
    std::string FormatTime();

    Level logLevel_;
    std::string basename_;
    int line_;
    std::stringstream stream_;
};

Logging logging_;
};

}


#define LOG_DEBUG minlog::Logger(__FILE__, __LINE__, minlog::Logger::DEBUG).Stream()
#define LOG_INFO minlog::Logger(__FILE__, __LINE__, minlog::Logger::INFO).Stream()
#define LOG_WARN minlog::Logger(__FILE__, __LINE__, minlog::Logger::WARN).Stream()
#define LOG_ERROR minlog::Logger(__FILE__, __LINE__, minlog::Logger::ERROR).Stream()



#endif  //LOGGER_HPP_
