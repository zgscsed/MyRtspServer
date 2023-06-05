/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: Channel.cpp
 * @Author: zgscsed
 * @Date: 2023年5月27日22:12:27
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年5月27日22:12:30
 * @Description: Channel 类
	封装fd的读写处理函数， 需要设置读写事件的回调函数。

// EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
// EPOLLOUT：表示对应的文件描述符可以写；
// EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
// EPOLLERR：表示对应的文件描述符发生错误；
// EPOLLHUP：表示对应的文件描述符被挂断；
// EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
// EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里
 */

#include "Channel.hpp"

#include <sys/epoll.h>

Channel::Channel()
	:fd_(-1),
	 events_(-1)
{

}

Channel::~Channel()
{

}

// 设置文件描述符
void Channel::SetFd(int fd)
{
	fd_ = fd;
}
// 获取文件描述符
int Channel::GetFd()
{
	return fd_;
}

// 设置触发事件
void Channel::SetEvents(uint32_t events)
{
	events_ = events;
}

uint32_t Channel::GetEvents()
{
	return events_;
}

// 事件分发处理
void Channel::HandleEvent()
{
	if (events_ & EPOLLRDHUP)    // 异常关闭事件
	{
		std::cout << "Event EPOLLRDHUP" << std::endl;
		closeCallback_();
	}
	else if (events_ & (EPOLLIN | EPOLLPRI))   // 读事件， 对端有数据或者正常关闭
	{
		readCallback_();
	}
	else if (events_ & EPOLLOUT)   // 写事件
	{
		writeCallback_();
	}
	else
	{
		std::cout << "Event error" << std::endl;
		errorCallback_();
	}
}

// 设置读事件回调
void Channel::SetReadCallback(const Callback& cb)
{
	readCallback_ = cb;
}

// 设置写事件回调
void Channel::SetWriteCallback(const Callback& cb)
{
	writeCallback_ = cb;
}

// 设置错误事件回调
void Channel::SetErrorCallback(const Callback& cb)
{
	errorCallback_ = cb;
}

// 设置关闭事件回调
void Channel::SetCloseCallback(const Callback& cb)
{
	closeCallback_ = cb;
}