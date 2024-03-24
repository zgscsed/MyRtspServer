<!--
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: rtspSession.md
 * @Author: zgscsed
 * @Date: 2023-02-24 21:33:39
 * @LastEditors: zgscsed
 * @LastEditTime: 2023-02-24 21:40:05
 * @Description: rstp session class desc
-->

## RtspSession类设计

RtspSession类，管理rtsp会话，一个会话对应一个实例对象。

处理接收的客户端rtsp请求，并且回复响应。

### 关键函数 ###

**praseRtspRequest**

将字符串消息解析，并且保存到消息结构体中

**rtspProcess**

处理请求消息，并回复响应

## Socket类设计

Socket类，实现对套接字的封装， 包括 bind()、listen()、accept()、connect()、send()、recv() 和 setsockopt()基本函数。


## UsageEnvironment类设计
UsageEnvironment 类，环境变量类，提供服务运行的环境，事件调度，定时器，线程池等功能。

## EventScheduler类
EventScheduler 类，事件调度类，封装了事件循环类，负责socket监听，并且执行回调函数，支持添加异步事件。

## MediaSource类
MediaSource 类，视频源基类   
	提供获取视频源帧数据的接口GetFrame()  
	不同编码的视频源继承基类实现readFrame虚函数

## H264MediaSource类
H264MediaSource 类，h264格式的视频源
readFrame 函数读取视频源中的一帧h264数据

## TimerManager 类
定时器管理类，使用时间轮机制轮询定时器。

## RtpSink 类
获取视频源帧，封装成rtp格式的包，调用回调函数，发送

## H264RtpSink 类
获取视频源帧，封装成H264 rtp格式的包，调用回调函数，发送

## MediaSession 类
功能：一个session可以支持多个客户端拉流。保存了视频源，以及rtsp回话协商的媒体面参数。实现发送rtp流给客户端。

## Ipv4Address类
功能：封装IPv4网络序地址

## RtpEndPoint类
功能：RtpEndPoint类，实现rtp包的收发。
## RtspEndPoint类
功能：RtspEndPoint类，实现rtsp包的收发。