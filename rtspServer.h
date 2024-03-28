/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp服务器类，使用rtsp协议传输数据

类功能流程：tcp接收客户端连接，根据rtsp协议接收数据并解析发送到客户端，最后传输rtp数据

*/

#ifndef RTSPSERVER_H
#define RTSPSERVER_H

#include "net/UDPSocket.hpp"
#include "net/TCPSocket.hpp"
#include "net/TcpConnection.hpp"
#include "net/TcpServer.hpp"
#include "rtspSession.h"
#include "net/UsageEnvironment.hpp"
#include "rtsp/MediaSession.hpp"

#include <memory>
#include <mutex>
#include <map>
#define BUFSIZE 4096

class RtspServer {
public:
    //tcp连接的智能指针类型
    typedef std::shared_ptr<TcpConnection> spTcpConnection;
	RtspServer(UsageEnvironment* env, int serverport, int rtpPort, int rtcpPort);
	~RtspServer();

	//rtsp数据接收和发送过程
	void messagesProcess(int clientSockfd, char* clientIp);

    MediaSession* GetMediaSession(std::string name);

	void start();                        //启动
private:
    //新连接回调函数
    void HandleNewConnection(const spTcpConnection& sptcpconn);

    //数据接收回调函数
    void HandleMessage(const spTcpConnection& sptcpconn, std::string& msg);

    //数据发送完成回调函数
    void HandleSendComplete(const spTcpConnection& sptcpconn);

    //连接关闭回调函数
    void HandleClose(const spTcpConnection& sptcpconn);

    //连接异常回调函数
    void HandleError(const spTcpConnection& sptcpconn);

    std::map<spTcpConnection, std::shared_ptr<RtspSession>> rtspSessionMap_;
    std::mutex mutex_;       // 可能多线程使用上面的数据

    TcpServer tcpServer_;          //使用tcp服务器管理链接

    std::map<std::string, MediaSession*> mediaSessions_;

    UsageEnvironment* env_;
	//RtspSession* session;                         //解析消息

};
#endif // !RTSPSERVER_H
