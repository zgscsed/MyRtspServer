/*
Copyright
time: 2021.4.26
author:zhoudong
desc: rtsp�������ĻỰ�����࣬ʵ�ֽ���rstp����

*/

#include "rtspSession.h"
#include <sstream>
#include <cstring> 
RtspSession::RtspSession(int rtpPort, int rtcpPort)
	: serverRtpFd_(),
	serverRtcpFd_()
{
	serverRtpFd_.setReuseAddr();
	serverRtpFd_.bindAddr(rtpPort);

	serverRtcpFd_.setReuseAddr();
	serverRtcpFd_.bindAddr(rtcpPort);

	this->serverRtpPort_ = rtpPort;
	this->serverRtcpPort_ = rtcpPort;
}
RtspSession::~RtspSession()
{
	serverRtpFd_.close();
	serverRtcpFd_.close();
}

//������Ϣ
bool RtspSession::praseRtspRequest(std::string&msg, RtspRequestContext & rtspRequestContext)
{
/*
�����ʽ��
method url vesion\r\n        //������
CSeq: x\r\n                  //�ײ���, Ҳ������ͷ
xxx\r\n
...
\r\n                         //�ײ���

//ʵ��
*/
	std::string crlf("\r\n");           //������Ϣ�ķָ���
	std::string crlfcrlf("\r\n\r\n");

	int prev = 0;
	int next = 0;

	if ((next = msg.find(crlf, prev)) != msg.npos)
	{
		//�ҵ�crlf���ֵ�λ��,��ȡ����������Ϣ
		std::string request_line(msg.substr(prev, next - prev));
		std::stringstream iss(request_line);
		iss >> rtspRequestContext.method;
		iss >> rtspRequestContext.url;
		iss >> rtspRequestContext.version;

		prev = next;
	}
	else
	{
		std::cout << "rtsp request request_line error" << std::endl;
		return false;
	}

	//��������ͷ
	std::string key, value;
	int pos_crlfcrlf = 0;
	if ((pos_crlfcrlf = msg.find(crlfcrlf, prev)) != msg.npos)       //ȷ��crlfcrlf��λ��
	{
		while (prev != pos_crlfcrlf)        //���µ�λ��û��ָ��CRLFCRLF��˵���м�������ͷ����
		{
			//�ҵ���һ��
			int pos = msg.find(crlf, prev + 2);      //prev+2ԭ����prevָ��crlf�� һ���������ַ�����Ҫ����
			int pos_colon = msg.find(":", prev + 2);    //ȷ��ð�ŵ�λ��
			key = msg.substr(prev + 2, pos_colon - prev - 2);
			value = msg.substr(pos_colon + 2, pos - pos_colon - 2);    //ð�ź��пո�

			rtspRequestContext.header.insert(std::pair<std::string, std::string>(key, value));

			prev = pos;

		}
	}
	else
	{
		std::cout << "rtsp request request header error" << std::endl;
		return false;
	}

	//����ʵ��
	rtspRequestContext.body = msg.substr(pos_crlfcrlf + 4);

	msg.clear();

	return true;
}

//������Ϣ
void RtspSession::rtspProcess(const RtspRequestContext& rtspRequestContext, std::string&responseContext)
{
	char result[100];
	std::string cseqs("Cseq");
	auto iter =  rtspRequestContext.header.find(cseqs);
	int cseq = atoi(iter->second.c_str());
	if (rtspRequestContext.method == "OPTIONS")
	{
		sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Public: OPTIONS, DESCRIBE, SETUP, PLAY\r\n"
			"\r\n",
			cseq);
	}
	else if (rtspRequestContext.method == "DESCRIBE")
	{
		char sdp[500];
		char localIp[100];

		sscanf(rtspRequestContext.url.c_str(), "rtsp://%[^:]:", localIp);

		sprintf(sdp, "v=0\r\n"
			"o=- 9%ld 1 IN IP4 %s\r\n"
			"t=0 0\r\n"
			"a=control:*\r\n"
			"m=video 0 RTP/AVP 96\r\n"
			"a=rtpmap:96 H264/90000\r\n"
			"a=control:track0\r\n",
			time(NULL), localIp);

		sprintf(result, "RTSP/1.0 200 OK\r\nCSeq: %d\r\n"
			"Content-Base: %s\r\n"
			"Content-type: application/sdp\r\n"
			"Content-length: %d\r\n\r\n"
			"%s",
			cseq,
			rtspRequestContext.url.c_str(),
			strlen(sdp),
			sdp);
	}
	else if (rtspRequestContext.method == "SETUP")
	{
		//�Ƚ����ͻ���rtp�˿ں�
		auto it = rtspRequestContext.header.find("Transport");         //�ҵ�Transport
		if (it != rtspRequestContext.header.end())
		{
			//���ַ����У�ȡ����Ӧ��ֵ
			sscanf(it->second.c_str(), "Transport: RTP/AVP;unicast;client_port=%d-%d\r\n",
				&clientRtpPort_, &clientRtcpPort_);
		}

		sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Transport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d\r\n"
			"Session: 66334873\r\n"
			"\r\n",
			cseq,
			clientRtpPort_,
			clientRtcpPort_,
			serverRtpPort_,
			serverRtcpPort_);
	}
	else if(rtspRequestContext.method == "PLAY")
	{
		sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Range: npt=0.000-\r\n"
			"Session: 66334873; timeout=60\r\n\r\n",
			cseq);
	}

	//����
	if (rtspRequestContext.method == "PLAY")
	{

	}

	responseContext = "";
	responseContext = std::string(result);
}