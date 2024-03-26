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
#include "rtsp/H264RtpSink.hpp"
#include "rtsp/MediaSession.hpp"
#include "rtsp/H264MediaSource.hpp"
#include "net/EventScheduler.hpp"
#include "net/UsageEnvironment.hpp"

int main(void)
{
	// 初始化事件循环类, 后面还需要添加定时器
	EventLoop* loop = new EventLoop();
	EventScheduler* scheduler = new EventScheduler(loop);

	// 线程池
	ThreadPool* pool = ThreadPool::Create(4);
	// 定时器
	int interval = 1;
	int slotnum = 1024;
	TimerManager* timerMgr = TimerManager::Create(interval, slotnum);

	// 初始化环境变量
	UsageEnvironment* env = UsageEnvironment::CreateNew(scheduler, pool, timerMgr);

	MediaSession* session = MediaSession::Create(env, "live");
	// 视频源
	MediaSource* mediaSource = H264MediaSource::Create(env, "test.h264");
	// rtpSink 初始化
	H264RtpSink* h264RtpSink = H264RtpSink::Create(env, mediaSource);

	session->AddRtpSink(1, h264RtpSink);


	std::cout << "RTSP服务器项目" << std::endl;

	// RtspServer 重新写
	RtspServer rtspServer(env, 8554, 55532, 55533);

	rtspServer.start();
	
	env->Scheduler()->Loop();

	return 0;
}