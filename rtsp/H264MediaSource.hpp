/*
 * Copyright (C) 2024 zgscsed. All rights reserved.
 * @filename: H264MediaSource.hpp
 * @Author: zgscsed
 * @Date: 2024年3月20日21:13:02
 * @LastEditors: zgscsed
 * @LastEditTime: 2024年3月20日21:13:02
 * @Description: H264MediaSource 类
	媒体源类，提供h264视频源帧数据。
 */

#ifndef H264_MEDIASOURCE_HPP_
#define H264_MEDIASOURCE_HPP_

#include "MediaSource.hpp"

class H264MediaSource : public MediaSource
{
public:
	static H264MediaSource* Create(UsageEnvironment* env, std::string filename);

	
	~H264MediaSource();

	int Init();

	virtual void ReadFrame();

private:
	H264MediaSource(UsageEnvironment* env, std::string filename);

	bool StartCode3(uint8_t* buf);
	bool StartCode4(uint8_t* buf);
	uint8_t* FindNextStartCode(uint8_t* buf, int len);
	int GetFrameFromH264File(int fd, uint8_t* frame, int size);
	std::string  filename_;
	int fd_;    // 视频源文件描述符

};

#endif   // !H264_MEDIASOURCE_HPP_