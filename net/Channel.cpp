/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: Channel.cpp
 * @Author: zgscsed
 * @Date: 2023��5��27��22:12:27
 * @LastEditors: zgscsed
 * @LastEditTime: 2023��5��27��22:12:30
 * @Description: Channel ��
	��װfd�Ķ�д�������� ��Ҫ���ö�д�¼��Ļص�������

// EPOLLIN ����ʾ��Ӧ���ļ����������Զ��������Զ�SOCKET�����رգ���
// EPOLLOUT����ʾ��Ӧ���ļ�����������д��
// EPOLLPRI����ʾ��Ӧ���ļ��������н��������ݿɶ�������Ӧ�ñ�ʾ�д������ݵ�������
// EPOLLERR����ʾ��Ӧ���ļ���������������
// EPOLLHUP����ʾ��Ӧ���ļ����������Ҷϣ�
// EPOLLET�� ��EPOLL��Ϊ��Ե����(Edge Triggered)ģʽ�����������ˮƽ����(Level Triggered)��˵�ġ�
// EPOLLONESHOT��ֻ����һ���¼���������������¼�֮���������Ҫ�����������socket�Ļ�����Ҫ�ٴΰ����socket���뵽EPOLL������
 */

#include "Channel.hpp"

#include <sys/epoll.h>

Channel::Channel()
	:fd_(-1),
	 events_(-1)
{

}

Channel::~Channel()
{

}

// �����ļ�������
inline void Channel::SetFd(int fd)
{
	fd_ = fd;
}
// ��ȡ�ļ�������
inline int Channel::GetFd() const
{
	return fd_;
}

// ���ô����¼�
inline void Channel::SetEvents(uint32_t events)
{
	events_ = events;
}

inline uint32_t Channel::GetEvents() const
{
	return events_;
}

// �¼��ַ�����
void Channel::HandleEvent()
{
	if (events_ & EPOLLRDHUP)    // �쳣�ر��¼�
	{
		std::cout << "Event EPOLLRDHUP" << std::endl;
		closeCallback_();
	}
	else if (events_ & (EPOLLIN | EPOLLPRI))   // ���¼��� �Զ������ݻ��������ر�
	{
		readCallback_();
	}
	else if (events_ & EPOLLOUT)   // д�¼�
	{
		writeCallback_();
	}
	else
	{
		std::cout << "Event error" << std::endl;
		errorCallback_();
	}
}

// ���ö��¼��ص�
inline void Channel::SetReadCallback(const Callback& cb)
{
	readCallback_ = cb;
}

// ����д�¼��ص�
inline void Channel::SetWriteCallback(const Callback& cb)
{
	writeCallback_ = cb;
}

// ���ô����¼��ص�
inline void Channel::SetErrorCallback(const Callback& cb)
{
	errorCallback_ = cb;
}

// ���ùر��¼��ص�
inline void Channel::SetCloseCallback(const Callback& cb)
{
	closeCallback_ = cb;
}