/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: Eventpoll.cpp
 * @Author: zgscsed
 * @Date: 2023年5月15日21:41:33
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年5月15日21:41:40
 * @Description: epoll 类
	封装epoll的三个关键函数，便于使用epoll。

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
const int MAX_EVENT_NUM = 4096;    // 最大触发事件数量
const int TIME_OUT = 1000;         // epoll_wait 超时时间设置

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

// 等待事件， epoll_wait函数封装
void EventPoll::Poll(ChannelList& activeChannelList)
{
    int timeout = TIME_OUT;
    int nfds = epoll_wait(pollfd_, &*eventList_.begin(), (int)eventList_.capacity(), timeout);

    if (nfds == -1)
    {
        std::cout << "epoll_wait error"  << std::endl;
    }

    // 便利事件列表，设置activeChannelList。每个Channel设置好events。根据events选择对应的回调函数执行
    for (int i = 0; i < nfds; ++i)
    {
        int events = eventList_[i].events;

    }
}

// 添加事件监听, EPOLL_CTL_ADD
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

// 删除事件监听, EPOLL_CTL_DEL
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

// 修改事件监听, EPOLL_CTL_MOD
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