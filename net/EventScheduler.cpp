/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: EventScheduler.cpp
 * @Author: zgscsed
 * @Date: 2023年8月21日21:44:26
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年8月21日21:44:31
 * @Description: EventScheduler 类
	事件调度类，调度当前所有事件。
 */

#include "EventScheduler.hpp"

EventScheduler::EventScheduler(EventLoop* loop)
	:loop_(loop)
{

}
EventScheduler::~EventScheduler()
{

}

EventLoop* EventScheduler::Loop()
{
	return loop_;
}

void EventScheduler::Quit()
{
	loop_->Quit();
}