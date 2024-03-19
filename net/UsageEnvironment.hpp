/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: UsageEnvironment.hpp
 * @Author: zgscsed
 * @Date: 2023年8月23日21:17:29
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年8月23日21:17:32
 * @Description: UsageEnvironment 类
	环境信息类，管理事件调度，提供接口给应用调用。
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
