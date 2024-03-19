/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: EventScheduler.cpp
 * @Author: zgscsed
 * @Date: 2023��8��21��21:44:26
 * @LastEditors: zgscsed
 * @LastEditTime: 2023��8��21��21:44:31
 * @Description: EventScheduler ��
	�¼������࣬���ȵ�ǰ�����¼���
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