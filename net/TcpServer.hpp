/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: TcpServer.hpp
 * @Author: zgscsed
 * @Date: 2023年6月5日21:54:25
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年6月5日21:54:29
 * @Description: TcpServer 类
	tcp服务器类
 */

#ifndef TCP_SERVER_HPP_
#define TCP_SERVER_HPP_

#include "TcpConnection.hpp"
#include "TCPSocket.hpp"
#include "Channel.hpp"
#include "EventLoopThreadPool.hpp"
#define MAXCONNECTION 20000
class TcpServer {
public:
	// TcpConnection 智能指针
	using spTcpConnection = std::shared_ptr<TcpConnection>;

	// 回调函数类型
	using Callback = std::function<void(const spTcpConnection&)>;
	using MessageCallback = std::function<void(const spTcpConnection&, std::string&)>;

	// loop 可以用线程池
	TcpServer(EventLoop* loop, const int port, const int threadNum = 0);
	~TcpServer();

	// 启动
	void Start();

	// 注册新的连接回调函数
	void SetNewConnCallback(const Callback& cb);

	// 注册数据回调函数, 具体业务服务器处理消息的函数
	void SetMessageCallback(const MessageCallback& cb);

	// 注册数据发送完成回调函数
	void SetSendCompleteCallback(const Callback& cb);

	// 连接关闭回调函数
	void SetCloseCallback(const Callback& cb);

	// 连接异常回调函数
	void SetErrorCallback(const Callback& cb);
private:
	// 服务器socket
	TCPSocket serverSocket_;

	// 主loop
	EventLoop* loop_;

	// 服务器事件
	Channel serverChannel_;

	// 连接数量统计
	int connCount_;

	// tcp连接表
	std::map<int, std::shared_ptr<TcpConnection>> tcpConnList_;

	// 保护tcp连接表的互斥量
	std::mutex mutex_;

	// io线程池
	EventLoopThreadPool eventLoopThreadPool_;
	// 业务接口回调函数
	Callback newConnectionCallback_;
	MessageCallback messageCallback_;
	Callback sendCompleteCallback_;
	Callback closeCallback_;
	Callback errorCallback_;

	// 新连接处理函数
	void OnNewConnection();

	// 移除tcp连接函数
	void RemoveConnection(const std::shared_ptr<TcpConnection> spTcpConnection);

	// 服务器socket的异常处理函数
	void OnConnectionError();
};

#endif // !TCP_SERVER_HPP_
