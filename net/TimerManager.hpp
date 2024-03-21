/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: TimerManager.hpp
 * @Author: zgscsed
 * @Date: 2024年3月21日23:38:19
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月21日23:38:19
 * @Description: TimerManager 类
	定时器管理类，添加删除定时器， 
	以及定时执行超时的定时器.基于时间轮实现，增加删除O(1),
	执行可能复杂度高些，slot多的话可以降低链表长度。

	缺点：只支持单线程，单个定时器执行时间过长会影响其它定时器
 */

#ifndef TIMER_MANAGER_HPP_
#define TIMER_MANAGER_HPP_

#include "Timer.hpp"

#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
class TimerManager
{
public:
	static TimerManager* Create(int interval, int slotNum);

	~TimerManager();

	// 添加定时器
	void AddTimer(Timer* pTimer);

	// 删除定时器
	void RemoveTimer(Timer* pTimer);

	// 调整定时器
	void AdjustTimer(Timer* pTimer);
	// 启动
	void Start();
	// 停止
	void Stop();



private:
	TimerManager(int interval, int slotNum);

	// 时间轮实现
	int currentSlot_;
	// slot的时间间隔 ms
	int slotInterval_;
	// slot总数
	int slotNum_;
	// 时间轮结构
	std::vector<Timer*> timeWheel_;

	std::mutex timeWheelMutex_;
	// 时间轮运行状态
	bool running_;

	// 定时器线程
	std::thread th_;

	// 时间轮操作函数
	// 检查超时任务
	void CheckTimer();

	// 线程实际执行的函数
	void CheckTick();

	// 计算定时器参数
	void CalculateTimer(Timer* pTimer);

	// 添加定时器到时间轮
	void AddTimerToTimeWheel(Timer* pTimer);

	// 从时间轮中移除定时器
	void RemoveTimerFromTimeWheel(Timer* pTimer);

	// 从时间轮中修改定时器
	void AdjustTimerToTimeWheel(Timer* pTimer);

};


#endif   // !TIMER_MANAGER_HPP_

