/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: Timer.hpp
 * @Author: zgscsed
 * @Date: 2024��3��21��23:22:47
 * @LastEditors: zgscsed
 * @LastEditTime: 2024��3��21��23:22:47
 * @Description: Timer ��
	��ʱ���࣬ ��ʱִ�лص�����
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_
#include <functional>

class Timer
{
public:
	using Callback = std::function<void()>;

	// ��ʱ������
	enum class TimerType
	{
		TIMER_ONCE,
		TIMER_PERIOD
	};

	Timer(int timeout, TimerType timerType, Callback timercallback);
	~Timer();

	// �������ö�ʱ��
	void Adjust(int timeout, TimerType timerType, Callback timercallback);

	void Start();   // ��ʱ������
	void Stop();    // ��ʱ��ֹͣ���Ӷ�ʱ����������ɾ��

	// ��ʱ����ʱʱ�� ��λms
	int timeout_;

	// ��ʱ������
	TimerType timerType_;

	// �ص�����
	Callback timerCallback_;

	// ��ʱ��ʣ�µ�ת��
	int rotation_;

	// ��ʱ�����ڵ�ʱ���
	int timeslot_;

	// ��ʱ������ָ��
	Timer* prev_;
	Timer* next_;
};

#endif   // !TIMER_HPP_

