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

UsageEnvironment* UsageEnvironment::CreateNew(EventScheduler* scheduler)
{
	if (scheduler == nullptr)
	{
		return nullptr;
	}

	return new UsageEnvironment(scheduler);
}
UsageEnvironment::UsageEnvironment(EventScheduler* scheduler)
{
	scheduler_ = scheduler;
}
UsageEnvironment::~UsageEnvironment()
{
	scheduler_->Quit();
}

EventScheduler* UsageEnvironment::Scheduler()
{
	return scheduler_;
}