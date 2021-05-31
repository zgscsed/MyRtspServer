#ifndef UDPSOCKET_H
#define UDPSOCKET_H

/*
Copyright
time: 2021.4.25
author:zhoudong
desc: ��������socket��,udp���䣬��װsocket�������ͳ�ʼ������

*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class UdpSocket {
public:
	UdpSocket();
	~UdpSocket();

	//���õ�ַ����
	void setReuseAddr();

	//�󶨵�ַ
	bool bindAddr(int port);

	//����fd
	int getFd() const { return fd_; }

	//�ر�fd
	bool close();

	//�жϴ����׽����Ƿ�ɹ�
	bool isCreate();

	//�����ǰ�׽���ip�Ͷ˿�
	void printIPAndPort();

private:
	int fd_;
};

#endif // !UDPSOCKET_H

