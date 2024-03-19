/*
Copyright
2021.4.23
author:zhoudong
desc:
*/
#include <iostream>
#include <fstream>
#include "rtspServer.h"
#include "net/EventLoop.hpp"
#include "net/EventScheduler.hpp"
#include "net/UsageEnvironment.hpp"

int main(void)
{
	// 初始化事件循环类, 后面还需要添加定时器
	EventLoop* loop = new EventLoop();
	EventScheduler* scheduler = new EventScheduler(loop);

	// 初始化环境变量
	UsageEnvironment* env = UsageEnvironment::CreateNew(scheduler);
	std::cout << "RTSP服务器项目" << std::endl;

	// RtspServer 重新写
	RtspServer rtspServer(8554, 55532, 55533);

	rtspServer.start();
	
	env->Scheduler()->Loop();

	return 0;
}