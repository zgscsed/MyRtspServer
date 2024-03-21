/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: TimerManager.hpp
 * @Author: zgscsed
 * @Date: 2024��3��21��23:38:19
 * @LastEditors: zgscsed
 * @LastEditTime: 2024��3��21��23:38:19
 * @Description: TimerManager ��
	��ʱ�������࣬���ɾ����ʱ����
	�Լ���ʱִ�г�ʱ�Ķ�ʱ��.����ʱ����ʵ�֣�����ɾ��O(1),
	ִ�п��ܸ��Ӷȸ�Щ��slot��Ļ����Խ���������
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

// ��Ӷ�ʱ��
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

// ɾ����ʱ��
void TimerManager::RemoveTimer(Timer* pTimer)
{
	if (pTimer == nullptr)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(timeWheelMutex_);
	RemoveTimerFromTimeWheel(pTimer);
}

// ������ʱ��
void TimerManager::AdjustTimer(Timer* pTimer)
{
	if (pTimer == nullptr)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(timeWheelMutex_);
	AdjustTimerToTimeWheel(pTimer);
}
// ����
void TimerManager::Start()
{
	running_ = true;
	th_ = std::thread(&TimerManager::CheckTick, this);
}
// ֹͣ
void TimerManager::Stop()
{
	running_ = false;
	if (th_.joinable())
	{
		th_.join();
	}
}
// ��鳬ʱ����
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
			// ִ������
			pTimer->timerCallback_();
			// ɾ��
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
	currentSlot_ = (++currentSlot_) % slotNum_;   // ��һ����
}

// �߳�ʵ��ִ�еĺ���
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
		tickCount = (time - oldTime) / slotInterval_;         // ��������ʱ����ռ����slot
		oldTime = oldTime + tickCount * slotInterval_;

		for (int i = 0; i < tickCount; ++i)
		{
			CheckTimer();
		}
		std::this_thread::sleep_for(std::chrono::microseconds(500));       // ʱ������ԽС����ʱԽ����ȷ
	}
}

// ���㶨ʱ������, ���ݳ�ʱʱ���㣬��ʱ�����е�λ��
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
		tick = 1;       // ����һ��slot���,����ʱ1��slot����
	}
	else
	{
		tick = timeout / slotInterval_;

		pTimer->rotation_ = tick / slotNum_;
		int timeslot = (currentSlot_ + tick) % slotNum_;
		pTimer->timeslot_ = timeslot;
	}
}

// ��Ӷ�ʱ����ʱ����
void TimerManager::AddTimerToTimeWheel(Timer* pTimer)
{
	if (pTimer == nullptr)
	{
		return;
	}

	int timeslot = pTimer->timeslot_;
	// ʱ������Ѿ����ڶ�ʱ��������������
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

// ��ʱ�������Ƴ���ʱ��
void TimerManager::RemoveTimerFromTimeWheel(Timer* pTimer)
{
	if (pTimer == nullptr)
	{
		return;
	}

	int timeslot = pTimer->timeslot_;

	if (pTimer == timeWheel_[timeslot])
	{
		// ��һ�����
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

// ��ʱ�������޸Ķ�ʱ��
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