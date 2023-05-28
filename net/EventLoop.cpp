/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: EventLoop.cpp
 * @Author: zgscsed
 * @Date: 2023年5月28日16:54:01
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年5月28日16:54:05
 * @Description: EventLoop 类
	io复用处理流程抽象，等待事件，处理事件。执行任务
 */


#include "EventLoop.hpp"
#include <sys/eventfd.h>
#include <unistd.h>

 //参照muduo，实现跨线程唤醒
int CreateEventFd()
{
	int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		std::cout << "Failed in eventfd" << std::endl;
		exit(1);
	}
	return evtfd;
}
EventLoop::EventLoop()
	:functorList_(),
	 channelList_(),
	 activeChannelList_(),
	 poller_(),
	 quit_(true),
	 tid_(std::this_thread::get_id()),
	 mutex_(),
	 wakeUpFd_(CreateEventFd()),
	 wakeUpChannel_()
{
	wakeUpChannel_.SetFd(wakeUpFd_);
	wakeUpChannel_.SetEvents(EPOLLIN | EPOLLET);
	wakeUpChannel_.SetReadCallback(std::bind(&EventLoop::ReadHandle, this));
	wakeUpChannel_.SetErrorCallback(std::bind(&EventLoop::ErrorHandle, this));
	AddChannelToEventPoller(&wakeUpChannel_);
}
EventLoop::~EventLoop()
{
	close(wakeUpFd_);
}


// 执行事件循环
void EventLoop::Loop()
{
	quit_ = false;
	while (!quit_)
	{
		poller_.Poll(activeChannelList_);
		for (Channel* pChannel : activeChannelList_)
		{
			pChannel->HandleEvent();       // 处理事件
		}

		activeChannelList_.clear();

		ExecuteTask();       // 执行任务
	}
}

// 添加事件
inline void EventLoop::AddChannelToEventPoller(Channel* pChannel)
{
	poller_.AddChannel(pChannel);
}

// 删除事件
inline void EventLoop::DeleteChannelToEventPoller(Channel* pChannel)
{
	poller_.DeleteChannel(pChannel);
}

// 修改事件
inline void EventLoop::ModifyChannelToEventPoller(Channel* pChannel)
{
	poller_.ModifyChannel(pChannel);
}

// 退出循环
inline void EventLoop::Quit()
{
	quit_ = true;
}

// 获取线程id
inline std::thread::id EventLoop::GetThreadId() const
{
	return tid_;
}

// 唤醒loop
void EventLoop::WakeUp()
{
	uint64_t one = 1;
	ssize_t n = write(wakeUpFd_, (char*)(&one), sizeof(one));
}

// 唤醒后回调
void EventLoop::ReadHandle()
{
	uint64_t one = 1;
	ssize_t n = read(wakeUpFd_, &one, sizeof(one));
}
// 唤醒后错误处理
void EventLoop::ErrorHandle()
{
	;
}

// 添加任务
void EventLoop::AddTask(Functor functor)
{
	{
		std::lock_guard<std::mutex> lock(mutex_);
		functorList_.emplace_back(functor);
	}

	WakeUp();         // 跨线程唤醒， worker线程唤醒io线程
}

// 执行任务
void EventLoop::ExecuteTask()
{
	std::vector<Functor> functorList;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		functorList.swap(functorList_);
	}

	for (Functor& functor : functorList)
	{
		functor();
	}

	functorList.clear();
}