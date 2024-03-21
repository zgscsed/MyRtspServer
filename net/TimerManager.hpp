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
	ִ�п��ܸ��Ӷȸ�Щ��slot��Ļ����Խ��������ȡ�

	ȱ�㣺ֻ֧�ֵ��̣߳�������ʱ��ִ��ʱ�������Ӱ��������ʱ��
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

	// ��Ӷ�ʱ��
	void AddTimer(Timer* pTimer);

	// ɾ����ʱ��
	void RemoveTimer(Timer* pTimer);

	// ������ʱ��
	void AdjustTimer(Timer* pTimer);
	// ����
	void Start();
	// ֹͣ
	void Stop();



private:
	TimerManager(int interval, int slotNum);

	// ʱ����ʵ��
	int currentSlot_;
	// slot��ʱ���� ms
	int slotInterval_;
	// slot����
	int slotNum_;
	// ʱ���ֽṹ
	std::vector<Timer*> timeWheel_;

	std::mutex timeWheelMutex_;
	// ʱ��������״̬
	bool running_;

	// ��ʱ���߳�
	std::thread th_;

	// ʱ���ֲ�������
	// ��鳬ʱ����
	void CheckTimer();

	// �߳�ʵ��ִ�еĺ���
	void CheckTick();

	// ���㶨ʱ������
	void CalculateTimer(Timer* pTimer);

	// ��Ӷ�ʱ����ʱ����
	void AddTimerToTimeWheel(Timer* pTimer);

	// ��ʱ�������Ƴ���ʱ��
	void RemoveTimerFromTimeWheel(Timer* pTimer);

	// ��ʱ�������޸Ķ�ʱ��
	void AdjustTimerToTimeWheel(Timer* pTimer);

};


#endif   // !TIMER_MANAGER_HPP_

