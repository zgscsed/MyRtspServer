/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: Timer.hpp
 * @Author: zgscsed
 * @Date: 2024年3月21日23:22:47
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月21日23:22:47
 * @Description: Timer 类
	定时器类， 超时执行回调函数
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_
#include <functional>

class Timer
{
public:
	using Callback = std::function<void()>;

	// 定时器类型
	enum class TimerType
	{
		TIMER_ONCE,
		TIMER_PERIOD
	};

	Timer(int timeout, TimerType timerType, Callback timercallback);
	~Timer();

	// 重新设置定时器
	void Adjust(int timeout, TimerType timerType, Callback timercallback);

	void Start();   // 定时器启动
	void Stop();    // 定时器停止，从定时器管理类中删除

	// 定时器超时时间 单位ms
	int timeout_;

	// 定时器类型
	TimerType timerType_;

	// 回调函数
	Callback timerCallback_;

	// 定时器剩下的转数
	int rotation_;

	// 定时器所在的时间槽
	int timeslot_;

	// 定时器链表指针
	Timer* prev_;
	Timer* next_;
};

#endif   // !TIMER_HPP_

