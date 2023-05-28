/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: EventLoop.cpp
 * @Author: zgscsed
 * @Date: 2023��5��28��16:54:01
 * @LastEditors: zgscsed
 * @LastEditTime: 2023��5��28��16:54:05
 * @Description: EventLoop ��
	io���ô������̳��󣬵ȴ��¼��������¼���ִ������
 */


#include "EventLoop.hpp"
#include <sys/eventfd.h>
#include <unistd.h>

 //����muduo��ʵ�ֿ��̻߳���
int CreateEventFd()
{
	int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		std::cout << "Failed in eventfd" << std::endl;
		exit(1);
	}
	return evtfd;
}
EventLoop::EventLoop()
	:functorList_(),
	 channelList_(),
	 activeChannelList_(),
	 poller_(),
	 quit_(true),
	 tid_(std::this_thread::get_id()),
	 mutex_(),
	 wakeUpFd_(CreateEventFd()),
	 wakeUpChannel_()
{
	wakeUpChannel_.SetFd(wakeUpFd_);
	wakeUpChannel_.SetEvents(EPOLLIN | EPOLLET);
	wakeUpChannel_.SetReadCallback(std::bind(&EventLoop::ReadHandle, this));
	wakeUpChannel_.SetErrorCallback(std::bind(&EventLoop::ErrorHandle, this));
	AddChannelToEventPoller(&wakeUpChannel_);
}
EventLoop::~EventLoop()
{
	close(wakeUpFd_);
}


// ִ���¼�ѭ��
void EventLoop::Loop()
{
	quit_ = false;
	while (!quit_)
	{
		poller_.Poll(activeChannelList_);
		for (Channel* pChannel : activeChannelList_)
		{
			pChannel->HandleEvent();       // �����¼�
		}

		activeChannelList_.clear();

		ExecuteTask();       // ִ������
	}
}

// ����¼�
inline void EventLoop::AddChannelToEventPoller(Channel* pChannel)
{
	poller_.AddChannel(pChannel);
}

// ɾ���¼�
inline void EventLoop::DeleteChannelToEventPoller(Channel* pChannel)
{
	poller_.DeleteChannel(pChannel);
}

// �޸��¼�
inline void EventLoop::ModifyChannelToEventPoller(Channel* pChannel)
{
	poller_.ModifyChannel(pChannel);
}

// �˳�ѭ��
inline void EventLoop::Quit()
{
	quit_ = true;
}

// ��ȡ�߳�id
inline std::thread::id EventLoop::GetThreadId() const
{
	return tid_;
}

// ����loop
void EventLoop::WakeUp()
{
	uint64_t one = 1;
	ssize_t n = write(wakeUpFd_, (char*)(&one), sizeof(one));
}

// ���Ѻ�ص�
void EventLoop::ReadHandle()
{
	uint64_t one = 1;
	ssize_t n = read(wakeUpFd_, &one, sizeof(one));
}
// ���Ѻ������
void EventLoop::ErrorHandle()
{
	;
}

// �������
void EventLoop::AddTask(Functor functor)
{
	{
		std::lock_guard<std::mutex> lock(mutex_);
		functorList_.emplace_back(functor);
	}

	WakeUp();         // ���̻߳��ѣ� worker�̻߳���io�߳�
}

// ִ������
void EventLoop::ExecuteTask()
{
	std::vector<Functor> functorList;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		functorList.swap(functorList_);
	}

	for (Functor& functor : functorList)
	{
		functor();
	}

	functorList.clear();
}