/*
Copyright
2021.4.23
author:zhoudong
desc:
*/
#include <iostream>
#include <fstream>
#include "rtspServer.h"

int main(void)
{
	std::cout << "RTSP服务器项目" << std::endl;

	RtspServer rtspServer(8554, 55532, 55533);

	rtspServer.start();

	int i = 0;
	std::cin >> i;

	return 0;
}