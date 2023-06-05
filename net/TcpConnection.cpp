/*
 * Copyright (C) 2023 zgscsed. All rights reserved.
 * @filename: TcpConnection.cpp
 * @Author: zgscsed
 * @Date: 2023��5��28��21:38:02
 * @LastEditors: zgscsed
 * @LastEditTime: 2023��5��28��21:38:06
 * @Description: TcpConnection ��
	һ���ͻ��˵����ӳ���
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
	// ����channel �¼�ִ�к���
	spChannel_->SetFd(fd_);
	spChannel_->SetEvents(EPOLLIN | EPOLLET);
	spChannel_->SetReadCallback(std::bind(&TcpConnection::HandleRead, this));
	spChannel_->SetWriteCallback(std::bind(&TcpConnection::HandleWrite, this));
	spChannel_->SetErrorCallback(std::bind(&TcpConnection::HandleError, this));
	spChannel_->SetCloseCallback(std::bind(&TcpConnection::HandleClose, this));
}
TcpConnection::~TcpConnection()
{
	// ɾ���¼�
	loop_->DeleteChannelToEventPoller(spChannel_.get());
	close(fd_);
}

// ��ȡ�ļ�������
int TcpConnection::GetFd() const
{
	return fd_;
}

// ��ȡloop
EventLoop* TcpConnection::GetLoop() const
{
	return loop_;
}

// ������Ӷ�Ӧ���¼���loop
void TcpConnection::AddChannelToLoop()
{
	// ���̣߳�ͨ���������ִ��
	loop_->AddTask(std::bind(&EventLoop::AddChannelToEventPoller, loop_, spChannel_.get()));
}

// ������Ϣ
void TcpConnection::Send(const std::string& s)
{
	bufferOut_ += s;       // ���͵���Ϣ����
	if (loop_->GetThreadId() == std::this_thread::get_id())
	{
		SendInLoop();
	}
	else
	{
		asyncProcessing_ = false;    // �첽���ý���
		loop_->AddTask(std::bind(&TcpConnection::SendInLoop, shared_from_this()));   // ���̵߳��ã�����io�߳��������
	}
}

// ��io�̷߳�������
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
		if (bufferOut_.size() > 0)        // ����û�з����꣬����Ҫ����
		{
			spChannel_->SetEvents(events | EPOLLOUT);
			loop_->ModifyChannelToEventPoller(spChannel_.get());
		}
		else
		{
			// ���ݷ�����
			spChannel_->SetEvents(events & (~EPOLLOUT));
			sendCompleteCallback_(shared_from_this());

			if (halfClose_)
			{
				// ��رգ����������ݣ��ر�
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
		// ��������ر�
		HandleClose();
	}
}

// ������������
void TcpConnection::Shutdown()
{
	if (loop_->GetThreadId() == std::this_thread::get_id())
	{
		ShutdownInLoop();
	}
	else
	{
		// ����io�̣߳����̵߳���
		loop_->AddTask(std::bind(&TcpConnection::ShutdownInLoop, shared_from_this()));
	}
}

// ��io�߳���������
void TcpConnection::ShutdownInLoop()
{
	if (disConnected_)
	{
		return;
	}
	std::cout << "shutdown" << std::endl;
	closeCallback_(shared_from_this());    // Ӧ�ò�����������ӻص�
	loop_->AddTask(std::bind(connectionCleanUp_, shared_from_this()));       // �������Լ�������Ҫloopִ�У�tcpserver����tcpconnection
}

// �ɶ��¼�
void TcpConnection::HandleRead()
{
	int ret = recvn(fd_, bufferIn_);

	if (ret > 0)
	{
		// ����
		messageCallback_(shared_from_this(), bufferIn_);
	}
	else if (ret == 0)
	{
		// �������ݳ���Ϊ0�� ��ʾ��Ҫ�ر�
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
			// ����û�з����꣬���������ˣ�����һ��EPOLLOUT�¼�����
			spChannel_->SetEvents(events | EPOLLOUT);
			loop_->ModifyChannelToEventPoller(spChannel_.get());      // ����ֱ���޸�
		}
		else
		{
			// ���ݷ�����, ȡ��EPOLLOUT�¼�
			spChannel_->SetEvents(events & (~EPOLLOUT));
			sendCompleteCallback_(shared_from_this());
			// ����Ѿ��ǰ�ر�״̬��close
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

	// ����
	loop_->AddTask(std::bind(connectionCleanUp_, shared_from_this()));   // ��Ҫloop�����Լ����������Լ�
	disConnected_ = true;
}
void TcpConnection::HandleClose()
{
	// �Զ˹ر����ӣ������֣�һ��close��һ����ر�
	if (disConnected_)
	{
		return;
	}

	// ����������ݣ���������ٹر�
	if (bufferOut_.size() > 0 || bufferIn_.size() > 0 || asyncProcessing_)
	{
		// ����������ݣ����ð�رգ��ȷ���
		halfClose_ = true;
		// �������ݸղ��յ��������յ�FIN
		if (bufferIn_.size() > 0)
		{
			messageCallback_(shared_from_this(), bufferIn_);
		}
	}
	else
	{
		// ������������Ȼ��ص�close�� �������ӹر�
		loop_->AddTask(std::bind(connectionCleanUp_, shared_from_this()));
		closeCallback_(shared_from_this());
		disConnected_ = true;
	}
}

// �յ����ݻص�����
void TcpConnection::SetMessageCallback(const MessageCallback& cb)
{
	this->messageCallback_ = cb;
}
// ���������ݻص�����
void TcpConnection::SetSendCompleteCallback(const Callback& cb)
{
	this->sendCompleteCallback_ = cb;
}
// ���ӹرջص�����
void TcpConnection::SetCloseCallback(const Callback& cb)
{
	this->closeCallback_ = cb;
}
// �����쳣�ص�����
void TcpConnection::SetErrorCallback(const Callback& cb)
{
	this->errorCallback_ = cb;
}
// ��������������
void TcpConnection::SetConnectionCleanUp(const Callback& cb)
{
	connectionCleanUp_ = cb;
}

// �����첽�����־�����������̳߳ص�ʱ��ʹ��
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
			bufferin.append(buffer, nbyte);//Ч�ʽϵͣ�2�ο���
			readsum += nbyte;
			if (nbyte < BUFSIZE)
				return readsum;//���Ż�����Сһ�ζ����ã���Ϊһ�ε��ú�ʱ10+us
			else
				continue;
		}
		else if (nbyte < 0)//�쳣
		{
			if (errno == EAGAIN)//ϵͳ������δ�����ݣ�����������
			{
				//std::cout << "EAGAIN,ϵͳ������δ�����ݣ�����������" << std::endl;
				return readsum;
			}
			else if (errno == EINTR)
			{
				std::cout << "errno == EINTR" << std::endl;
				continue;
			}
			else
			{
				//������RST
				perror("recv error");
				//std::cout << "recv error" << std::endl;
				return -1;
			}
		}
		else//����0���ͻ��˹ر�socket��FIN
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
	//�޿����Ż�
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
		else if (nbyte < 0)//�쳣
		{
			if (errno == EAGAIN)//ϵͳ��������������������
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
				//�ͻ����Ѿ�close��������RST������wirte�ᱨEPIPE������0����ʾclose
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
		else//����0
		{
			//Ӧ�ò��᷵��0
			//std::cout << "client close the Socket!" << std::endl;
			return 0;
		}
	}
}