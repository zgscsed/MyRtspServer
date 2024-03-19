/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: UsageEnvironment.hpp
 * @Author: zgscsed
 * @Date: 2023��8��23��21:17:29
 * @LastEditors: zgscsed
 * @LastEditTime: 2023��8��23��21:17:32
 * @Description: UsageEnvironment ��
	������Ϣ�࣬�����¼����ȣ��ṩ�ӿڸ�Ӧ�õ��á�
 */

#ifndef USAGE_ENVIRONMENT_HPP_
#define USAGE_ENVIRONMENT_HPP_

#include "EventScheduler.hpp"
class UsageEnvironment {
public:
	static UsageEnvironment* CreateNew(EventScheduler* scheduler);
	UsageEnvironment(EventScheduler* scheduler);
	~UsageEnvironment();

	EventScheduler* Scheduler();

private:
	EventScheduler* scheduler_;
};
#endif // !USAGE_ENVIRONMENT_HPP_
