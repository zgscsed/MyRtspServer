/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: Channel.hpp
 * @Author: zgscsed
 * @Date: 2023年5月15日21:41:33
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年5月15日21:41:40
 * @Description: Channel 类
	封装fd的读写处理函数， 需要设置读写事件的回调函数。
 */

#ifndef CHANNEL_HPP_
#define CHANNEL_HPP_

#include <iostream>
#include <functional>

class Channel {
public:

	// 回调函数类型定义
	using Callback = std::function<void()>;

	Channel();
	~Channel();

	// 设置文件描述符
	void SetFd(int fd);
	// 获取文件描述符
	int GetFd();

	// 设置触发事件
	void SetEvents(uint32_t events);
	uint32_t GetEvents();

	// 事件分发处理
	void HandleEvent();

	// 设置读事件回调
	void SetReadCallback(const Callback& cb);

	// 设置写事件回调
	void SetWriteCallback(const Callback& cb);

	// 设置错误事件回调
	void SetErrorCallback(const Callback& cb);

	// 设置关闭事件回调
	void SetCloseCallback(const Callback& cb);

private:
	// 文件描述符
	int fd_;
	// 事件，使用epoll时是epoll事件
	uint32_t events_;

	// 事件触发的执行函数，在tcpconn中注册
	Callback readCallback_;
	Callback writeCallback_;
	Callback errorCallback_;
	Callback closeCallback_;
};

#endif // !CHANNEL_HPP_
