/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: TcpConnection.cpp
 * @Author: zgscsed
 * @Date: 2023年5月28日21:38:02
 * @LastEditors: zgscsed
 * @LastEditTime: 2023年5月28日21:38:06
 * @Description: TcpConnection 类
	一个客户端的连接抽象
 */

#include "TcpConnection.hpp"

#include <iostream>
#include <unistd.h>

#define BUFSIZE 4096

int recvn(int fd, std::string& bufferin);
int sendn(int fd, std::string& bufferout);
TcpConnection::TcpConnection(EventLoop* loop, int fd, const struct sockaddr_in& clientAddr)
	:loop_(loop),
	fd_(fd),
	spChannel_(new Channel()),
	clientAddr_(clientAddr),
	halfClose_(false),
	disConnected_(false),
	asyncProcessing_(false),
	bufferIn_(),
	bufferOut_()
{
	// 设置channel 事件执行函数
	spChannel_->SetFd(fd_);
	spChannel_->SetEvents(EPOLLIN | EPOLLET);
	spChannel_->SetReadCallback(std::bind(&TcpConnection::HandleRead, this));
	spChannel_->SetWriteCallback(std::bind(&TcpConnection::HandleWrite, this));
	spChannel_->SetErrorCallback(std::bind(&TcpConnection::HandleError, this));
	spChannel_->SetCloseCallback(std::bind(&TcpConnection::HandleClose, this));
}
TcpConnection::~TcpConnection()
{
	// 删除事件
	loop_->DeleteChannelToEventPoller(spChannel_.get());
	close(fd_);
}

// 获取文件描述符
int TcpConnection::GetFd() const
{
	return fd_;
}

// 获取loop
EventLoop* TcpConnection::GetLoop() const
{
	return loop_;
}

// 添加连接对应的事件到loop
void TcpConnection::AddChannelToLoop()
{
	// 多线程，通过任务队列执行
	loop_->AddTask(std::bind(&EventLoop::AddChannelToEventPoller, loop_, spChannel_.get()));
}

// 发生消息
void TcpConnection::Send(const std::string& s)
{
	bufferOut_ += s;       // 发送的消息缓存
	if (loop_->GetThreadId() == std::this_thread::get_id())
	{
		SendInLoop();
	}
	else
	{
		asyncProcessing_ = false;    // 异步调用结束
		loop_->AddTask(std::bind(&TcpConnection::SendInLoop, shared_from_this()));   // 跨线程调用，加入io线程任务队列
	}
}

// 在io线程发生数据
void TcpConnection::SendInLoop()
{
	if (disConnected_)
	{
		return;
	}
	int result = sendn(fd_, bufferOut_);
	if (result > 0)
	{
		uint32_t events = spChannel_->GetEvents();
		if (bufferOut_.size() > 0)        // 数据没有发生完，还需要发送
		{
			spChannel_->SetEvents(events | EPOLLOUT);
			loop_->ModifyChannelToEventPoller(spChannel_.get());
		}
		else
		{
			// 数据发送完
			spChannel_->SetEvents(events & (~EPOLLOUT));
			sendCompleteCallback_(shared_from_this());

			if (halfClose_)
			{
				// 半关闭，发送完数据，关闭
				HandleClose();
			}
		}
	}
	else if (result < 0)
	{
		HandleError();
	}
	else
	{
		// 其他情况关闭
		HandleClose();
	}
}

// 主动清理连接
void TcpConnection::Shutdown()
{
	if (loop_->GetThreadId() == std::this_thread::get_id())
	{
		ShutdownInLoop();
	}
	else
	{
		// 不是io线程，跨线程调用
		loop_->AddTask(std::bind(&TcpConnection::ShutdownInLoop, shared_from_this()));
	}
}

// 在io线程清理连接
void TcpConnection::ShutdownInLoop()
{
	if (disConnected_)
	{
		return;
	}
	std::cout << "shutdown" << std::endl;
	closeCallback_(shared_from_this());    // 应用层调用清理连接回调
	loop_->AddTask(std::bind(connectionCleanUp_, shared_from_this()));       // 清理不能自己清理，需要loop执行，tcpserver清理tcpconnection
}

// 可读事件
void TcpConnection::HandleRead()
{
	int ret = recvn(fd_, bufferIn_);

	if (ret > 0)
	{
		// 接受
		messageCallback_(shared_from_this(), bufferIn_);
	}
	else if (ret == 0)
	{
		// 接受数据长度为0， 表示需要关闭
		HandleClose();
	}
	else
	{
		HandleError();
	}
}
void TcpConnection::HandleWrite()
{
	int ret = sendn(fd_, bufferOut_);
	if (ret > 0)
	{
		uint32_t events = spChannel_->GetEvents();
		if (bufferOut_.size() > 0)
		{
			// 数据没有发送完，缓冲区满了，设置一个EPOLLOUT事件触发
			spChannel_->SetEvents(events | EPOLLOUT);
			loop_->ModifyChannelToEventPoller(spChannel_.get());      // 这里直接修改
		}
		else
		{
			// 数据发送完, 取消EPOLLOUT事件
			spChannel_->SetEvents(events & (~EPOLLOUT));
			sendCompleteCallback_(shared_from_this());
			// 如果已经是半关闭状态，close
			if (halfClose_)
			{
				HandleClose();
			}
		}
	}
	else if (ret < 0)
	{
		HandleError();
	}
	else
	{
		HandleClose();
	}
}
void TcpConnection::HandleError()
{
	if (disConnected_)
	{
		return;
	}
	errorCallback_(shared_from_this());

	// 清理
	loop_->AddTask(std::bind(connectionCleanUp_, shared_from_this()));   // 需要loop清理，自己不能清理自己
	disConnected_ = true;
}
void TcpConnection::HandleClose()
{
	// 对端关闭连接，有两种，一个close，一个半关闭
	if (disConnected_)
	{
		return;
	}

	// 如果还有数据，发送完后再关闭
	if (bufferOut_.size() > 0 || bufferIn_.size() > 0 || asyncProcessing_)
	{
		// 如果还有数据，设置半关闭，先发完
		halfClose_ = true;
		// 还有数据刚才收到，但是收到FIN
		if (bufferIn_.size() > 0)
		{
			messageCallback_(shared_from_this(), bufferIn_);
		}
	}
	else
	{
		// 设置清理任务，然后回调close， 设置连接关闭
		loop_->AddTask(std::bind(connectionCleanUp_, shared_from_this()));
		closeCallback_(shared_from_this());
		disConnected_ = true;
	}
}

// 收到数据回调函数
void TcpConnection::SetMessageCallback(const MessageCallback& cb)
{
	this->messageCallback_ = cb;
}
// 发生完数据回调函数
void TcpConnection::SetSendCompleteCallback(const Callback& cb)
{
	this->sendCompleteCallback_ = cb;
}
// 连接关闭回调函数
void TcpConnection::SetCloseCallback(const Callback& cb)
{
	this->closeCallback_ = cb;
}
// 连接异常回调函数
void TcpConnection::SetErrorCallback(const Callback& cb)
{
	this->errorCallback_ = cb;
}
// 设置连接清理函数
void TcpConnection::SetConnectionCleanUp(const Callback& cb)
{
	connectionCleanUp_ = cb;
}

// 设置异步处理标志，开启工作线程池的时候使用
void TcpConnection::SetAsyncProcessing(const bool asyncprocessing)
{
	asyncProcessing_ = asyncprocessing;
}



int recvn(int fd, std::string& bufferin)
{
	int nbyte = 0;
	int readsum = 0;
	char buffer[BUFSIZE];
	for (;;)
	{
		//nbyte = recv(fd, buffer, BUFSIZE, 0);
		nbyte = read(fd, buffer, BUFSIZE);

		if (nbyte > 0)
		{
			bufferin.append(buffer, nbyte);//效率较低，2次拷贝
			readsum += nbyte;
			if (nbyte < BUFSIZE)
				return readsum;//读优化，减小一次读调用，因为一次调用耗时10+us
			else
				continue;
		}
		else if (nbyte < 0)//异常
		{
			if (errno == EAGAIN)//系统缓冲区未有数据，非阻塞返回
			{
				//std::cout << "EAGAIN,系统缓冲区未有数据，非阻塞返回" << std::endl;
				return readsum;
			}
			else if (errno == EINTR)
			{
				std::cout << "errno == EINTR" << std::endl;
				continue;
			}
			else
			{
				//可能是RST
				perror("recv error");
				//std::cout << "recv error" << std::endl;
				return -1;
			}
		}
		else//返回0，客户端关闭socket，FIN
		{
			//std::cout << "client close the Socket" << std::endl;
			return 0;
		}
	}
}

int sendn(int fd, std::string& bufferout)
{
	ssize_t nbyte = 0;
	int sendsum = 0;
	//char buffer[BUFSIZE+1];
	size_t length = 0;
	//length = bufferout.copy(buffer, BUFSIZE, 0);
	//buffer[length] = '\0';	
	// if(bufferout.size() >= BUFSIZE)
	// {
	// 	length =  BUFSIZE;
	// }
	// else
	// {
	// 	length =  bufferout.size();
	// }
	//无拷贝优化
	length = bufferout.size();
	if (length >= BUFSIZE)
	{
		length = BUFSIZE;
	}
	for (;;)
	{
		//nbyte = send(fd, buffer, length, 0);
		//nbyte = send(fd, bufferout.c_str(), length, 0);
		nbyte = write(fd, bufferout.c_str(), length);
		if (nbyte > 0)
		{
			sendsum += nbyte;
			bufferout.erase(0, nbyte);
			//length = bufferout.copy(buffer, BUFSIZE, 0);
			//buffer[length] = '\0';
			length = bufferout.size();
			if (length >= BUFSIZE)
			{
				length = BUFSIZE;
			}
			if (length == 0)
			{
				return sendsum;
			}
		}
		else if (nbyte < 0)//异常
		{
			if (errno == EAGAIN)//系统缓冲区满，非阻塞返回
			{
				std::cout << "write errno == EAGAIN,not finish!" << std::endl;
				return sendsum;
			}
			else if (errno == EINTR)
			{
				std::cout << "write errno == EINTR" << std::endl;
				continue;
			}
			else if (errno == EPIPE)
			{
				//客户端已经close，并发了RST，继续wirte会报EPIPE，返回0，表示close
				perror("write error");
				std::cout << "write errno == client send RST" << std::endl;
				return -1;
			}
			else
			{
				perror("write error");//Connection reset by peer
				std::cout << "write error, unknow error" << std::endl;
				return -1;
			}
		}
		else//返回0
		{
			//应该不会返回0
			//std::cout << "client close the Socket!" << std::endl;
			return 0;
		}
	}
}