/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: MediaSource.cpp
 * @Author: zgscsed
 * @Date: 2024年3月19日23:23:50
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月19日23:23:50
 * @Description: MediaSource 类
	媒体源类，提供视频源帧数据。
 */

#include "MediaSource.hpp"

MediaSource::MediaSource(UsageEnvironment* env)
{
	for (int i = 0; i < DEFAULT_FRAME_NUM; ++i)
	{
		frameInputQueue_.push(new AVFrame());
	}
}
MediaSource::~MediaSource()
{
	while (!frameInputQueue_.empty())
	{
		AVFrame* frame = frameInputQueue_.front();
		delete frame;
		frameInputQueue_.pop();
	}

	while (!frameOutputQueue_.empty())
	{
		AVFrame* frame = frameOutputQueue_.front();
		delete frame;
		frameOutputQueue_.pop();
	}
}

AVFrame* MediaSource::GetFrame()
{
	std::lock_guard<std::mutex> loch(mutex_);

	if (frameOutputQueue_.empty())
	{
		return nullptr;
	}

	AVFrame* frame = frameOutputQueue_.front();
	frameOutputQueue_.pop();

	return frame;
}
void MediaSource::PutFrame(AVFrame* frame)
{
	std::lock_guard<std::mutex> loch(mutex_);
	frameInputQueue_.push(frame);

	env_->CurThreadPool()->AddTask([this]() {
		this->ReadFrame();
	});
}
int MediaSource::GetFps() const
{
	return fps_;
}

void MediaSource::SetFps(int fps)
{
	fps_ = fps;
}