/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: TcpConnection.hpp
 * @Author: zgscsed
 * @Date: 2023年5月28日21:38:02
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年5月28日21:38:06
 * @Description: TcpConnection 类
	一个客户端的连接抽象
 */

#ifndef TCP_CONNECTIOS_HPP_
#define TCP_CONNECTIOS_HPP_

#include "Channel.hpp"
#include "EventLoop.hpp"

#include <string>
#include <thread>
#include <memory>
#include <functional>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
	using spTcpConnection = std::shared_ptr<TcpConnection>;
	// 回调函数类型
	using Callback = std::function<void(const spTcpConnection&)>;
	using MessageCallback = std::function<void(const spTcpConnection&, std::string&)>;

	TcpConnection(EventLoop* loop, int fd, const struct sockaddr_in& clientAddr);
	~TcpConnection();

	// 获取文件描述符
	int GetFd() const;
	struct sockaddr_in GetPeerAddr() { return clientAddr_; }

	// 获取loop
	EventLoop* GetLoop() const;

	// 添加连接对应的事件到loop
	void AddChannelToLoop();

	// 发生消息
	void Send(const std::string& s);

	// 在io线程发生数据
	void SendInLoop();

	// 主动清理连接
	void Shutdown();

	// 在io线程清理连接
	void ShutdownInLoop();

	// 可读事件
	void HandleRead();
	void HandleWrite();
	void HandleError();
	void HandleClose ();

	// 收到数据回调函数
	void SetMessageCallback(const MessageCallback& cb);
	// 发生完数据回调函数
	void SetSendCompleteCallback(const Callback& cb);
	// 连接关闭回调函数
	void SetCloseCallback(const Callback& cb);
	// 连接异常回调函数
	void SetErrorCallback(const Callback& cb);
	// 设置连接清理函数
	void SetConnectionCleanUp(const Callback& cb);

	// 设置异步处理标志，开启工作线程池的时候使用
	void SetAsyncProcessing(const bool asyncprocessing);

private:
	// 当前连接所在的loop
	EventLoop* loop_;

	// 当前连接事件
	std::unique_ptr<Channel> spChannel_;

	// 文件描述符
	int fd_;

	// 对端地址
	struct sockaddr_in clientAddr_;

	// 半关闭标志位
	bool halfClose_;

	// 连接关闭标志
	bool disConnected_;

	// 异步调用标志位，当工作任务交给线程池时，需要设置true, 任务完成回调置为false
	bool asyncProcessing_;

	// 读写缓存
	std::string bufferIn_;
	std::string bufferOut_;

	// 回调函数
	MessageCallback messageCallback_;
	Callback sendCompleteCallback_;
	Callback closeCallback_;
	Callback errorCallback_;
	Callback connectionCleanUp_;
};
#endif // !TCP_CONNECTIOS_HPP_
