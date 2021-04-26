#ifndef TCPSOCKET_H
#define TCPSOCKET_H

/*
Copyright
time: 2021.4.24
author:zhoudong
desc: 服务器端socket类，封装socket描述符和初始化操作

*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
class TcpSocket {
public:
	TcpSocket();
	~TcpSocket();

	//设置地址重用
	void setReuseAddr();

	//设置非阻塞
	void setNonBlocking();

	//地址绑定
	bool bindAddr(int serverport);

	//监听
	bool listen();

	//获取连接
	int accept(struct sockaddr_in &clientaddr);

	//返回fd
	int getFd() const { return fd_; }

	//关闭fd
	bool close();

	//判断创建套接字是否成功
	bool isCreate();

private:
	int fd_;
};
#endif // !TCPSOCKET_H
