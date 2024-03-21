/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: ThreadPool.cpp
 * @Author: zgscsed
 * @Date: 2023年6月11日20:33:14
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年6月11日20:33:19
 * @Description: ThreadPool 类
	线程池，执行一些定时任务。
 */
#include "ThreadPool.hpp"
#include "base/global.hpp"
#include <iostream>

ThreadPool* ThreadPool::Create(int threadNum)
{
	ThreadPool* pool = new ThreadPool(threadNum);
	return pool;
}
ThreadPool::ThreadPool(int threadNum)
	:started_(false),
	threadNum_(threadNum),
	threadList_(),
	taskQueue_(),
	mutex_(),
	condition_()
{
	LOG_INFO << "ThreadPool construct threadNum: " << threadNum;
}
ThreadPool::~ThreadPool()
{
	std::cout << "clean up the ThreadPool" << std::endl;
	Stop();
	for (int i = 0; i < threadNum_; ++i)
	{
		threadList_[i]->join();
	}
	for (int i = 0; i < threadNum_; ++i)
	{
		delete threadList_[i];
	}
	threadList_.clear();
}

// 启动线程池
void ThreadPool::Start()
{
	if (threadNum_ > 0)
	{
		started_ = true;
		for (int i = 0; i < threadNum_; ++i)
		{
			std::thread* pThread = new std::thread(&ThreadPool::ThreadFunc, this);
			threadList_.push_back(pThread);
		}
	}
	else
	{
		;
	}
}

// 暂停线程池
void ThreadPool::Stop()
{
	started_ = false;
	condition_.notify_all();
}


// 添加任务
void ThreadPool::AddTask(Task task)
{
	{
		std::lock_guard<std::mutex> lock(mutex_);
		taskQueue_.push(task);
	}
	condition_.notify_one();          // 唤醒等待队列
}

// 线程函数
void ThreadPool::ThreadFunc()
{
	std::thread::id tid = std::this_thread::get_id();
	std::stringstream sin;
	sin << tid;
	std::cout << "worker thread is runing: " << tid << std::endl;
	Task task;

	while (started_)
	{
		task = nullptr;
		{
			std::unique_lock<std::mutex> lock(mutex_);   // unique_lock 支持解锁又上锁
			while (taskQueue_.empty() && started_)
			{
				condition_.wait(lock);
			}
			// 唤醒后检查一下，防止假唤醒
			if (!started_)
			{
				break;
			}
			task = taskQueue_.front();
			taskQueue_.pop();
		}
		if (task)
		{
			try
			{
				task();
			}
			catch (std::bad_alloc& ba)
			{
				std::cout << "bad_alloc caught in ThreadPool::ThreadFunc task: " << ba.what() << '\n';
				while (1);
			}
		}
	}
}

// 获取线程数量
int ThreadPool::GetThreadNum()
{
	return threadNum_;
}