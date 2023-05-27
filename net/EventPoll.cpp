/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: Eventpoll.cpp
 * @Author: zgscsed
 * @Date: 2023��5��15��21:41:33
 * @LastEditors: zgscsed
 * @LastEditTime: 2023��5��15��21:41:40
 * @Description: epoll ��
	��װepoll�������ؼ�����������ʹ��epoll��

        //    typedef union epoll_data {
        //        void    *ptr;
        //        int      fd;
        //        uint32_t u32;
        //        uint64_t u64;
        //    } epoll_data_t;

        //    struct epoll_event {
        //        uint32_t     events;    // Epoll events
        //        epoll_data_t data;      //User data variable 
        //    };
 */

#include "EventPoll.hpp"

#include <cstdlib>    // exit
#include <unistd.h>  // close
const int MAX_EVENT_NUM = 4096;    // ��󴥷��¼�����
const int TIME_OUT = 1000;         // epoll_wait ��ʱʱ������

EventPoll::EventPoll()
    : pollfd_(-1),
     eventList_(MAX_EVENT_NUM),
     channelMap_(),
     mutex_()
{
    pollfd_ = epoll_create(256);
    if (pollfd_ == -1)
    {
        std::cout << "epoll_create error" << std::endl;
        exit(1);
    }

    std::cout << "epoll_create " << pollfd_ << std::endl;
}
EventPoll::~EventPoll()
{
    close(pollfd_);
}

// �ȴ��¼��� epoll_wait������װ
void EventPoll::Poll(ChannelList& activeChannelList)
{
    int timeout = TIME_OUT;
    int nfds = epoll_wait(pollfd_, &*eventList_.begin(), (int)eventList_.capacity(), timeout);

    if (nfds == -1)
    {
        std::cout << "epoll_wait error"  << std::endl;
    }

    // �����¼��б�����activeChannelList��ÿ��Channel���ú�events������eventsѡ���Ӧ�Ļص�����ִ��
    for (int i = 0; i < nfds; ++i)
    {
        int events = eventList_[i].events;

    }
}

// ����¼�����, EPOLL_CTL_ADD
void EventPoll::AddChannel(Channel* pChannel)
{
    int fd = pChannel->GetFd();
    struct epoll_event ev;
    ev.events = pChannel->GetEvents();
    // data
    ev.data.ptr = pChannel;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        channelMap_[fd] = pChannel;
    }

    if (epoll_ctl(pollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        std::cout << "epoll add error" << std::endl;
        return;
    }
}

// ɾ���¼�����, EPOLL_CTL_DEL
void EventPoll::DeleteChannel(Channel* pChannel)
{
    int fd = pChannel->GetFd();
    struct epoll_event ev;
    ev.events = pChannel->GetEvents();
    // data
    ev.data.ptr = pChannel;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        channelMap_[fd] = pChannel;
    }

    if (epoll_ctl(pollfd_, EPOLL_CTL_DEL, fd, &ev) == -1)
    {
        std::cout << "epoll delete error" << std::endl;
        return;
    }
}

// �޸��¼�����, EPOLL_CTL_MOD
void EventPoll::ModifyChannel(Channel* pChannel)
{
    int fd = pChannel->GetFd();
    struct epoll_event ev;
    ev.events = pChannel->GetEvents();
    // data
    ev.data.ptr = pChannel;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        channelMap_[fd] = pChannel;
    }

    if (epoll_ctl(pollfd_, EPOLL_CTL_MOD, fd, &ev) == -1)
    {
        std::cout << "epoll modify error" << std::endl;
        return;
    }
}