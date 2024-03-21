/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: Timer.cpp
 * @Author: zgscsed
 * @Date: 2024年3月21日23:22:47
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月21日23:22:47
 * @Description: Timer 类
	定时器类， 超时执行回调函数
 */
#include "Timer.hpp"

#include "base/global.hpp"

Timer::Timer(int timeout, TimerType timerType, Callback timercallback)
	:timeout_(timeout), timerType_(timerType), timerCallback_(timercallback)
{
	rotation_ = 0;
	timeslot_ = 0;
	prev_ = nullptr;
	next_ = nullptr;

	if (timeout_ < 0)
	{
		LOG_ERROR << "Timer construct error! timeout < 0";
		return;
	}
}
Timer::~Timer()
{
	Stop();
}

// 重新设置定时器
void Timer::Adjust(int timeout, TimerType timerType, Callback timerCallback)
{
	timeout_ = timeout;
	timerType_ = timerType;
	timerCallback_ = timerCallback;
}

void Timer::Start()
{

}

void Timer::Stop()
{

}