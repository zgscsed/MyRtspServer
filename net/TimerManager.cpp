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
	执行可能复杂度高些，slot多的话可以降低链表长度
 */
#include "TimerManager.hpp"
#include "base/global.hpp"
#include <chrono>
#include <unistd.h>
#include <sys/time.h>
TimerManager* TimerManager::Create(int interval, int slotNum)
{
	TimerManager* manager = new TimerManager(interval, slotNum);
	return manager;
}

TimerManager::TimerManager(int interval, int slotNum)
	:slotInterval_(interval), slotNum_(slotNum), th_()
{
	currentSlot_ = 0;
	timeWheel_ = std::vector<Timer*>(slotNum, nullptr);
	running_ = false;
	LOG_INFO << "TimerManager construct interval: " << interval << " slotNum: "<< slotNum;
}

TimerManager::~TimerManager()
{
	Stop();
}

// 添加定时器
void TimerManager::AddTimer(Timer* pTimer)
{
	if (pTimer == nullptr)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(timeWheelMutex_);
	CalculateTimer(pTimer);
	AddTimerToTimeWheel(pTimer);
}

// 删除定时器
void TimerManager::RemoveTimer(Timer* pTimer)
{
	if (pTimer == nullptr)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(timeWheelMutex_);
	RemoveTimerFromTimeWheel(pTimer);
}

// 调整定时器
void TimerManager::AdjustTimer(Timer* pTimer)
{
	if (pTimer == nullptr)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(timeWheelMutex_);
	AdjustTimerToTimeWheel(pTimer);
}
// 启动
void TimerManager::Start()
{
	running_ = true;
	th_ = std::thread(&TimerManager::CheckTick, this);
}
// 停止
void TimerManager::Stop()
{
	running_ = false;
	if (th_.joinable())
	{
		th_.join();
	}
}
// 检查超时任务
void TimerManager::CheckTimer()
{
	std::lock_guard<std::mutex> lock(timeWheelMutex_);
	Timer* pTimer = timeWheel_[currentSlot_];
	while (pTimer != nullptr)
	{
		if (pTimer->rotation_ > 0)
		{
			--pTimer->rotation_;
			pTimer = pTimer->next_;
		}
		else
		{
			// 执行任务
			pTimer->timerCallback_();
			// 删除
			if (pTimer->timerType_ == Timer::TimerType::TIMER_ONCE)
			{
				Timer* tmp = pTimer;
				pTimer = pTimer->next_;
				RemoveTimerFromTimeWheel(tmp);
			}
			else
			{
				Timer* tmp = pTimer;
				pTimer = pTimer->next_;
				AdjustTimerToTimeWheel(tmp);
				if (currentSlot_ == tmp->timeslot_ && tmp->rotation_ > 0)
				{
					--tmp->rotation_;
				}
			}
		}
	}
	currentSlot_ = (++currentSlot_) % slotNum_;   // 下一个槽
}

// 线程实际执行的函数
void TimerManager::CheckTick()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	int oldTime = (tv.tv_sec % 10000) * 1000 + tv.tv_usec / 1000;
	int time;
	int tickCount;
	while (running_)
	{
		gettimeofday(&tv, nullptr);
		time = (tv.tv_sec % 10000) * 1000 + tv.tv_usec / 1000;
		tickCount = (time - oldTime) / slotInterval_;         // 计算两次时间间隔占几个slot
		oldTime = oldTime + tickCount * slotInterval_;

		for (int i = 0; i < tickCount; ++i)
		{
			CheckTimer();
		}
		std::this_thread::sleep_for(std::chrono::microseconds(500));       // 时间粒度越小，延时越不精确
	}
}

// 计算定时器参数, 根据超时时间算，在时间轮中的位置
void TimerManager::CalculateTimer(Timer* pTimer)
{
	if (pTimer == nullptr)
	{
		return;
	}

	int tick = 0;
	int timeout = pTimer->timeout_;

	if (timeout < slotInterval_)
	{
		tick = 1;       // 不足一个slot间隔,按延时1个slot计算
	}
	else
	{
		tick = timeout / slotInterval_;

		pTimer->rotation_ = tick / slotNum_;
		int timeslot = (currentSlot_ + tick) % slotNum_;
		pTimer->timeslot_ = timeslot;
	}
}

// 添加定时器到时间轮
void TimerManager::AddTimerToTimeWheel(Timer* pTimer)
{
	if (pTimer == nullptr)
	{
		return;
	}

	int timeslot = pTimer->timeslot_;
	// 时间槽上已经存在定时器，存入链表中
	if (timeWheel_[timeslot])
	{
		pTimer->next_ = timeWheel_[timeslot];
		timeWheel_[timeslot]->prev_ = pTimer;
		timeWheel_[timeslot] = pTimer;
	}
	else
	{
		timeWheel_[timeslot] = pTimer;
	}
}

// 从时间轮中移除定时器
void TimerManager::RemoveTimerFromTimeWheel(Timer* pTimer)
{
	if (pTimer == nullptr)
	{
		return;
	}

	int timeslot = pTimer->timeslot_;

	if (pTimer == timeWheel_[timeslot])
	{
		// 第一个结点
		timeWheel_[timeslot] = pTimer->next_;
		if (pTimer->next_ != nullptr)
		{
			pTimer->next_->prev_ = nullptr;
		}
		pTimer->prev_ = pTimer->next_ = nullptr;
	}
	else
	{
		if (pTimer->prev_ == nullptr)
		{
			return;
		}
		pTimer->prev_->next_ = pTimer->next_;
		if (pTimer->next_ != nullptr)
		{
			pTimer->next_->prev_ = pTimer->prev_;
		}

		pTimer->prev_ = pTimer->next_ = nullptr;
	}
}

// 从时间轮中修改定时器
void TimerManager::AdjustTimerToTimeWheel(Timer* pTimer)
{
	if (pTimer == nullptr)
	{
		return;
	}

	RemoveTimerFromTimeWheel(pTimer);
	CalculateTimer(pTimer);
	AddTimerToTimeWheel(pTimer);
}