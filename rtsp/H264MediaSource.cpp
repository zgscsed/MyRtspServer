/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: H264MediaSource.cpp
 * @Author: zgscsed
 * @Date: 2024年3月20日21:13:02
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月20日21:13:02
 * @Description: H264MediaSource 类
	媒体源类，提供h264视频源帧数据。
 */

#include "H264MediaSource.hpp"
#include "../base/global.hpp"
#include <fcntl.h>
#include <unistd.h>

H264MediaSource* H264MediaSource::Create(UsageEnvironment* env, std::string filename)
{

}

H264MediaSource::H264MediaSource(UsageEnvironment* env, std::string filename)
	: MediaSource(env), filename_(filename)
{
	fd_ = YZ_ERROR;
}

int H264MediaSource::Init()
{
	fd_ = ::open(filename_.c_str(), O_RDONLY);
	if (fd_ == -1)
	{
		LOG_ERROR << "open file fail!  filename: " << filename_;
		return YZ_ERROR;
	}

	// 先获取数据
	for (int i = 0; i < DEFAULT_FRAME_NUM; ++i)
	{
		env_->CurThreadPool()->AddTask([this]() {
			this->ReadFrame();
		});
	}
}

H264MediaSource::~H264MediaSource()
{
	::close(fd_);
}

/*
* 检测frameInputQueue_ 不为空，直接返回
* 读取一帧数据,保存到frameOutputQueue_队列中
*/
void H264MediaSource::ReadFrame()
{
	std::lock_guard<std::mutex> loch(mutex_);
	if (frameInputQueue_.empty())
	{
		return;
	}
	AVFrame* frame = frameInputQueue_.front();

	frame->frameSize_ = GetFrameFromH264File(fd_, frame->buffer_, FRAME_MAX_SIZE);
	if (frame->frameSize_ > 0)
	{
		LOG_ERROR << "GetFrameFromH264File fail!";
		return;
	}

	// 移除h264帧开始的编码
	if (StartCode3(frame->buffer_))
	{
		frame->frame_ = frame->buffer_ + 3;
		frame->frameSize_ -= 3;
	}
	else
	{
		frame->frame_ = frame->buffer_ + 4;
		frame->frameSize_ -= 4;
	}

	frameInputQueue_.pop();
	frameOutputQueue_.push(frame);
}

bool H264MediaSource::StartCode3(uint8_t* buf)
{
	if (buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool H264MediaSource::StartCode4(uint8_t* buf)
{
	if (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
uint8_t* H264MediaSource::FindNextStartCode(uint8_t* buf, int len)
{
	int i = 0;

	if (len < 3)
	{
		return nullptr;
	}

	for (i = 0; i < len - 3; ++i)
	{
		if (StartCode3(buf) || StartCode4(buf))
		{
			return buf;
		}

		++buf;
	}

	if (StartCode3(buf))
	{
		return buf;
	}

	return nullptr;
}
int H264MediaSource::GetFrameFromH264File(int fd, uint8_t* frame, int size)
{
	int rSize = 0;
	int frameSize = 0;
	uint8_t* nextStartCode;

	if (fd < 0)
	{
		return fd;
	}

	rSize = ::read(fd, frame, size);
	// h264 每帧开始 00 00 00 01  或者 00 00 01
	if (!StartCode3(frame) && !StartCode4(frame))
	{
		return -1;
	}

	nextStartCode = FindNextStartCode(frame + 3, rSize - 3);
	if (!nextStartCode)
	{
		return -1;
	}
	else
	{
		frameSize = (nextStartCode - frame);
		lseek(fd, frameSize - rSize, SEEK_CUR);
	}
}