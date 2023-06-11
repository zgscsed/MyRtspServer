/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: EventLoopThreadPool.cpp
 * @Author: zgscsed
 * @Date: 2023年6月11日20:33:14
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年6月11日20:33:19
 * @Description: EventLoopThreadPool 类
	event loop 线程池。
 */

#include "EventLoopThreadPool.hpp"

#include <iostream>

EventLoopThreadPool::EventLoopThreadPool(EventLoop* mainLoop, int threadNum)
	:mainLoop_(mainLoop),
	threadNum_(threadNum),
	threadList_(),
	index_(0)
{
	for (int i = 0; i < threadNum_; ++i)
	{
		EventLoopThread* pEventLoopThread = new EventLoopThread;
		threadList_.push_back(pEventLoopThread);
	}
}

EventLoopThreadPool::~EventLoopThreadPool()
{
	std::cout << "clean up the EventLoopThreadPool" << std::endl;
	for (int i = 0; i < threadNum_; ++i)
	{
		delete threadList_[i];
	}

	threadList_.clear();
}

// 启动线程池
void EventLoopThreadPool::Start()
{
	if (threadNum_ > 0)
	{
		for (int i = 0; i < threadNum_; ++i)
		{
			threadList_[i]->Start();
		}
	}
	else
	{
		;
	}
}

// 获取下一个被分发的loop, 依据rr轮询策略
EventLoop* EventLoopThreadPool::GetNextLoop()
{
	if (threadNum_ > 0)
	{
		EventLoop* loop = threadList_[index_]->GetLoop();
		index_ = (index_ + 1) % threadNum_;
	}
	else
	{
		return mainLoop_;
	}
}