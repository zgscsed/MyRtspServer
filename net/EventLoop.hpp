/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: EventLoop.hpp
 * @Author: zgscsed
 * @Date: 2023年5月28日16:54:01
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年5月28日16:54:05
 * @Description: EventLoop 类
	io复用处理流程抽象，等待事件，处理事件。执行任务
 */

#ifndef EVENT_LOOP_HPP_
#define EVENT_LOOP_HPP_

#include "EventPoll.hpp"

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <functional>


class EventLoop {
public:
	using Functor = std::function<void()>;
	using ChannelList = std::vector<Channel*>;
	EventLoop();
	~EventLoop();


	// 执行事件循环
	void Loop();

	// 添加事件
	void AddChannelToEventPoller(Channel* pChannel);

	// 删除事件
	void DeleteChannelToEventPoller(Channel* pChannel);

	// 修改事件
	void ModifyChannelToEventPoller(Channel* pChannel);

	// 退出循环
	void Quit();

	// 获取线程id
	std::thread::id GetThreadId() const;

	// 唤醒loop
	void WakeUp();

	// 唤醒后回调
	void ReadHandle();
	// 唤醒后错误处理
	void ErrorHandle();

	// 添加任务
	void AddTask(Functor funtor);

	// 执行任务
	void ExecuteTask();

private:
	// 任务列表
	std::vector<Functor> functorList_;
	// 事件列表
	ChannelList channelList_;

	// 活跃事件
	ChannelList activeChannelList_;

	// epoll
	EventPoll poller_;

	// 运行状态
	bool quit_;

	// 线程id
	std::thread::id tid_;

	std::mutex mutex_;

	// 跨线程唤醒fd
	int wakeUpFd_;

	// 用于唤醒当前loop的事件
	Channel wakeUpChannel_;
};




#endif  // !EVENT_LOOP_HPP_