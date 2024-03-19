/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: UsageEnvironment.cpp
 * @Author: zgscsed
 * @Date: 2023��8��23��21:17:22
 * @LastEditors: zgscsed
 * @LastEditTime: 2023��8��23��21:17:38
 * @Description: UsageEnvironment ��
	������Ϣ�࣬�����¼����ȣ��ṩ�ӿڸ�Ӧ�õ��á�
 */

#include "UsageEnvironment.hpp"

UsageEnvironment* UsageEnvironment::CreateNew(EventScheduler* scheduler, ThreadPool* pool)
{
	if (scheduler == nullptr)
	{
		return nullptr;
	}

	return new UsageEnvironment(scheduler, pool);
}
UsageEnvironment::UsageEnvironment(EventScheduler* scheduler, ThreadPool* pool)
{
	scheduler_ = scheduler;
	threadPool_ = pool;
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