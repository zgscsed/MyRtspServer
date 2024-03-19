/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: EventScheduler.hpp
 * @Author: zgscsed
 * @Date: 2023年8月21日21:44:26
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年8月21日21:44:31
 * @Description: EventScheduler 类
	事件调度类，调度当前所有事件。
 */

#ifndef EVENT_SCHEDULER_HPP_
#define EVENT_SCHEDULER_HPP_

#include "EventLoop.hpp"

class EventScheduler
{
public:
	explicit EventScheduler(EventLoop* loop);
	~EventScheduler();

	EventLoop* Loop();

	void Quit();

private:
	EventLoop* loop_;
};


#endif // !EVENT_SCHEDULER_HPP_