#ifndef TCPSOCKET_H
#define TCPSOCKET_H

/*
Copyright
time: 2021.4.24
author:zhoudong
desc: ��������socket�࣬��װsocket�������ͳ�ʼ������

*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
class TcpSocket {
public:
	TcpSocket();
	~TcpSocket();

	//���õ�ַ����
	void setReuseAddr();

	//���÷�����
	void setNonBlocking();

	//��ַ��
	bool bindAddr(int serverport);

	//����
	bool listen();

	//��ȡ����
	int accept(struct sockaddr_in &clientaddr);

	//����fd
	int getFd() const { return fd_; }

	//�ر�fd
	bool close();

	//�жϴ����׽����Ƿ�ɹ�
	bool isCreate();

private:
	int fd_;
};
#endif // !TCPSOCKET_H
