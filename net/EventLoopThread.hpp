/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: EventLoopThread.hpp
 * @Author: zgscsed
 * @Date: 2023年6月11日20:33:14
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年6月11日20:33:19
 * @Description: EventLoopThread 类
	封装thread 使用。
 */

#ifndef EVENT_LOOP_THREAD_HPP_
#define EVENT_LOOP_THREAD_HPP_

#include "EventLoop.hpp"

#include <thread>
#include <string>

class EventLoopThread {
public:
	EventLoopThread();
	~EventLoopThread();

	// 获取当前线程loop
	EventLoop* GetLoop();

	// 启动线程
	void Start();

	// 线程执行函数
	void ThreadFunc();

private:
	// c++线程
	std::thread th_;

	// 线程id
	std::thread::id threadId_;

	// 线程名字
	std::string threadName_;

	// loop
	EventLoop* loop_;
};

#endif // !EVENT_LOOP_THREAD_HPP_
