/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: EventLoopThreadPool.hpp
 * @Author: zgscsed
 * @Date: 2023年6月11日20:33:14
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年6月11日20:33:19
 * @Description: EventLoopThreadPool 类
	event loop 线程池。
 */

#ifndef EVENT_LOOP_THREAD_POOL_HPP_
#define EVENT_LOOP_THREAD_POOL_HPP_

#include "EventLoop.hpp"
#include "EventLoopThread.hpp"

#include <vector>
#include <string>

class EventLoopThreadPool {
public:
	EventLoopThreadPool(EventLoop* mainLoop, int threadNum = 0);
	~EventLoopThreadPool();

	// 启动线程池
	void Start();

	// 获取下一个被分发的loop, 依据rr轮询策略
	EventLoop* GetNextLoop();

private:
	// 主loop
	EventLoop* mainLoop_;

	// 线程数量
	int threadNum_;

	// 线程列表
	std::vector<EventLoopThread*> threadList_;

	// 用于轮询分发的索引
	int index_;
};


#endif // !EVENT_LOOP_THREAD_POOL_HPP_
