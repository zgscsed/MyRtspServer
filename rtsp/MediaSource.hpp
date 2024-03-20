/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: MediaSource.hpp
 * @Author: zgscsed
 * @Date: 2024年3月19日23:23:50
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月19日23:23:50
 * @Description: MediaSource 类
	媒体源类，提供视频源帧数据。
 */

#ifndef MEDIA_SOURCE_HPP_
#define MEDIA_SOURCE_HPP_

#include "../net/UsageEnvironment.hpp"

#include <mutex>
#include <queue>

const int FRAME_MAX_SIZE = 1024 * 500;
const int DEFAULT_FRAME_NUM = 4;         // frame变量 4个重复使用

// 定义frame
class AVFrame {
public:
	AVFrame()
		:buffer_(new uint8_t[FRAME_MAX_SIZE]),
		 frameSize_(0)
	{
	}
	~AVFrame()
	{
		delete buffer_;
	}

	uint8_t* buffer_;
	uint8_t* frame_;
	int frameSize_;
};

class MediaSource {
public:
	virtual ~MediaSource();

	AVFrame* GetFrame();
	void PutFrame(AVFrame* frame);
	int GetFps() const;

protected:
	MediaSource(UsageEnvironment* env);
	virtual void ReadFrame() = 0;
	void SetFps(int fps);


	UsageEnvironment* env_;
	int fps_;

	std::queue<AVFrame*> frameInputQueue_;     // 空闲的frame对象
	std::queue<AVFrame*> frameOutputQueue_;    // 存储已经读取的视频源帧
	std::mutex mutex_;


};

#endif // !MEDIA_SOURCE_HPP_
