/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: Eventpoll.hpp
 * @Author: zgscsed
 * @Date: 2023年5月15日21:41:33
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年5月15日21:41:40
 * @Description: epoll 类
	封装epoll的三个关键函数，便于使用epoll。
 */

#ifndef EVENT_POLL_HPP_
#define EVENT_POLL_HPP_


#include "Channel.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <mutex>

#include <sys/epoll.h>

class EventPoll {
public:
	// 事件指针数组类型
	using ChannelList = std::vector<Channel*>;

	EventPoll();
	~EventPoll();

	// 等待事件， epoll_wait函数封装
	void Poll(ChannelList& activeChannelList);

	// 添加事件监听, EPOLL_CTL_ADD
	void AddChannel(Channel* pChannel);

	// 删除事件监听, EPOLL_CTL_DEL
	void DeleteChannel(Channel* pChannel);

	// 修改事件监听, EPOLL_CTL_MOD
	void ModifyChannel(Channel* pChannel);

private:
	int pollfd_;        // epoll实例

	// events数组，传递给epoll_wait
	std::vector<struct epoll_event> eventList_;
	// 事件表
	std::map<int, Channel*> channelMap_;

	std::mutex mutex_;
};



#endif   // !EVENT_POLL_HPP_