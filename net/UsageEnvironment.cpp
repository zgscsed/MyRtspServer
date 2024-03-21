/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: UsageEnvironment.cpp
 * @Author: zgscsed
 * @Date: 2023年8月23日21:17:22
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年8月23日21:17:38
 * @Description: UsageEnvironment 类
	环境信息类，管理事件调度，提供接口给应用调用。
 */

#include "UsageEnvironment.hpp"

UsageEnvironment* UsageEnvironment::CreateNew(EventScheduler* scheduler, ThreadPool* pool, TimerManager* timerMgr)
{
	if (scheduler == nullptr)
	{
		return nullptr;
	}

	return new UsageEnvironment(scheduler, pool, timerMgr);
}
UsageEnvironment::UsageEnvironment(EventScheduler* scheduler, ThreadPool* pool, TimerManager* timerMgr)
{
	scheduler_ = scheduler;
	threadPool_ = pool;
	timerMgr_ = timerMgr;
}
UsageEnvironment::~UsageEnvironment()
{
	scheduler_->Quit();
	delete scheduler_;
	delete threadPool_;
}

EventScheduler* UsageEnvironment::Scheduler()
{
	return scheduler_;
}

ThreadPool* UsageEnvironment::CurThreadPool()
{
	return threadPool_;
}
TimerManager* UsageEnvironment::TimerMgr()
{
	return timerMgr_;
}
