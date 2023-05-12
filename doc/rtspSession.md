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
