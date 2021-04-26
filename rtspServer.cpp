/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp�������࣬ʹ��rtspЭ�鴫������

�๦�����̣�tcp���տͻ������ӣ�����rtspЭ��������ݲ��������͵��ͻ��ˣ������rtp����

*/
#include "rtspServer.h"
#include <iostream>

RtspServer::RtspServer(int serverport, int rtpPort, int rtcpPort)
	:serverSockfd_(),
	serverRtpFd_(),
	serverRtcpFd_()

{
	//���÷������׽���
	serverSockfd_.setReuseAddr();
	serverSockfd_.bindAddr(serverport);
	serverSockfd_.listen();

	serverRtpFd_.setReuseAddr();
	serverRtpFd_.bindAddr(rtpPort);

	serverRtcpFd_.setReuseAddr();
	serverRtcpFd_.bindAddr(rtcpPort);

}
RtspServer::~RtspServer()
{
	serverSockfd_.close();
	serverRtpFd_.close();
	serverRtcpFd_.close();
}

//����
void RtspServer::start()
{
	//��һ��
	if (!serverSockfd_.isCreate() || !serverRtpFd_.isCreate() || !serverRtcpFd_.isCreate())
	{
		//�����׽��������⣬ֱ�ӷ���
		return;
	}

	//�ڶ�������ʼ���տͻ��˵���������
	std::cout << "rtsp://" << std::endl;



}