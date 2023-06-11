/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: EventLoopThread.hpp
 * @Author: zgscsed
 * @Date: 2023年6月11日20:33:14
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年6月11日20:33:19
 * @Description: EventLoopThread 类
	封装thread 使用, 线程函数中执行loop。
 */
#include "EventLoopThread.hpp"

#include <iostream>
#include <sstream>

EventLoopThread::EventLoopThread()
	:th_(),
	threadId_(-1),
	threadName_("io thread"),
	loop_(nullptr)
{

}

EventLoopThread::~EventLoopThread()
{
	std::cout << "clean up the event loop thread id:" << std::this_thread::get_id() << std::endl;
	loop_->Quit();        // 停止线程
	th_.join();            // 线程结束后，清理
}

// 获取当前线程loop
EventLoop* EventLoopThread::GetLoop()
{
	return loop_;
}

// 启动线程
void EventLoopThread::Start()
{
	th_ = std::thread(&EventLoopThread::ThreadFunc, this);
}


// 线程执行函数
void EventLoopThread::ThreadFunc()
{
	EventLoop loop;
	loop_ = &loop;

	threadId_ = std::this_thread::get_id();
	std::stringstream sin;
	sin << threadId_;
	threadName_ += sin.str();
	
	std::cout << "in the thread: " << threadName_ << std::endl;

	try 
	{
		loop_->Loop();
	}
	catch (std::bad_alloc& ba)
	{
		std::cout << "bad_alloc caught in EventLoopThread::ThreadFunc loop:" << ba.what() << '\n';
	}
}