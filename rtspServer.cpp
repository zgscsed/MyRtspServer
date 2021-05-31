/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp�������࣬ʹ��rtspЭ�鴫������

�๦�����̣�tcp���տͻ������ӣ�����rtspЭ��������ݲ��������͵��ͻ��ˣ������rtp����

*/
#include "rtspServer.h"


#include <unistd.h>
#include <iostream>
#include <string>



//��������
int recvn(int fd, std::string&buf)
{
	int nbyte = 0;
	int readsum = 0;
	char buffer[BUFSIZE];

	readsum = read(fd, buffer, BUFSIZE);
	if (readsum < 0)
	{
		return readsum;
	}

	buf = buffer; 
	return readsum;
}

//��������
int sendn(int fd, std::string &buf)
{
	int length = buf.size();
	int nbyte = write(fd, buf.c_str(), length);

	return nbyte;
}

RtspServer::RtspServer(int serverport, int rtpPort, int rtcpPort)
	:serverSockfd_()
{
	//���÷������׽���
	serverSockfd_.setReuseAddr();
	serverSockfd_.bindAddr(serverport);
	serverSockfd_.listen();

	session = new RtspSession(rtpPort, rtcpPort);



}
RtspServer::~RtspServer()
{
	serverSockfd_.close();
	delete session;

}

//rtsp���ݽ��պͷ��͹���
void RtspServer::messagesProcess(int clientSockfd)
{

	RtspRequestContext rtspRequestContet;              //������Ϣ�ṹ��
	//RtspResponseContext rtspResponseContext;           //��Ӧ��Ϣ�ṹ��
	std::string rtspResponseContext;                   //��Ӧ��Ϣ
	std::string recvBuf;                               //������Ϣ
	std::string sendBuf;                               //������Ϣ

	//rtsp��������
	while (1)
	{
		//���տͻ��˵���Ϣ
		int readsum = recvn(clientSockfd, recvBuf);

		//û���յ����ݣ�˵�����������⣬ֱ���˳�
		if (readsum <= 0)
			break;

		std::cout << "----------------C->S---------------------------"<<std::endl;
		std::cout << recvBuf << std::endl;

		//������Ϣ
		session->praseRtspRequest(recvBuf, rtspRequestContet);

		//��Ϣ����
		session->rtspProcess(rtspRequestContet, rtspResponseContext);

		std::cout << "----------------S->C---------------------------" << std::endl;
		std::cout << rtspResponseContext << std::endl;


		//������Ϣ
		sendn(clientSockfd, rtspResponseContext);
	}

	//�رտͻ��˷��������
	::close(clientSockfd);
}

//����
void RtspServer::start()
{
	//��һ��
	if (!serverSockfd_.isCreate())
	{
		//�����׽��������⣬ֱ�ӷ���
		return;
	}

	//�ڶ�������ʼ���տͻ��˵���������
	serverSockfd_.printIPAndPort();

	while (1)
	{
		int clientSockfd;
		char clientIP[40];
		int clientPort;

		struct sockaddr_in clientaddr;

		//����һ���ͻ��˺�
		if ((clientSockfd = serverSockfd_.accept(clientaddr)) > 0)
		{
			//�����ͻ��˵�ip�Ͷ˿�
			std::cout << "new clinet from ip: " << inet_ntoa(clientaddr.sin_addr) 
				<< ":" << ntohs(clientaddr.sin_port) << std::endl;

			//ͨ�ţ�rtsp�Ľ�������
			messagesProcess(clientSockfd);

		}
	}



}