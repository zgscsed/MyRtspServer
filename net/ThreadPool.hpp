/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: ThreadPool.hpp
 * @Author: zgscsed
 * @Date: 2023年6月11日20:33:14
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年6月11日20:33:19
 * @Description: ThreadPool 类
	线程池，执行一些定时任务。
 */

#ifndef THREAD_POOL_HPP_
#define THREAD_POOL_HPP_

#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
	using Task = std::function<void()>;       // 执行任务，简单使用function实现

	ThreadPool(int threadNum = 0);
	~ThreadPool();

	// 启动线程池
	void Start();

	// 暂停线程池
	void Stop();

	// 添加任务
	void AddTask(Task task);

	// 线程函数
	void ThreadFunc();

	// 获取线程数量
	int GetThreadNum();
private:
	// 运行状态
	bool started_;

	// 线程数量
	int threadNum_;

	// 线程列表
	std::vector<std::thread*> threadList_;

	// 任务队列
	std::queue<Task> taskQueue_;

	// 任务队列互斥量
	std::mutex mutex_;

	// 任务队列同步的条件变量
	std::condition_variable condition_;
};

#endif // !THREAD_POOL_HPP_
