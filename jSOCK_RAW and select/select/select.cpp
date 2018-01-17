// EchoTCPServer-select.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#undef UNICODE 
#define WIN32_LEAN_AND_MEAN 
#include<iostream>
#include <windows.h> 
#include <winsock2.h> 
#include <ws2tcpip.h> 
#include <stdlib.h> 
#include <stdio.h> 

using namespace std;

// 连接到winsock2对应的lib文件： Ws2_32.lib 
#pragma comment (lib, "Ws2_32.lib") 

#define DEFAULT_BUFLEN 512    //默认缓冲区长度为512
#define DEFAULT_PORT 27015    //默认服务器端口号为27015
int main(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	int iResult;
	SOCKET ServerSocket = INVALID_SOCKET;
	SOCKET AcceptSocket = INVALID_SOCKET;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	sockaddr_in addrClient;
	int addrClientlen = sizeof(sockaddr_in);

	// 初始化 Winsock 
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	// 创建用于监听的套接字   
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (ServerSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// 为套接字绑定地址和端口号
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;
	//addrServ.sin_port = htons(DEFAULT_PORT);		// 监听端口为DEFAULT_PORT
	addrServ.sin_port = htons(1234);
	addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	iResult = bind(ServerSocket, (const struct sockaddr*)&addrServ, sizeof(SOCKADDR_IN));
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(ServerSocket);
		WSACleanup();
		return 1;
	}

	// 监听套接字   
	iResult = listen(ServerSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed !\n");
		closesocket(ServerSocket);
		WSACleanup();
		return -1;
	}
	printf("TCP server starting ...\n");
	fd_set fdRead, fdSocket;
	FD_ZERO(&fdSocket);
	FD_SET(ServerSocket, &fdSocket);


	//=====================================================================
	SOCKET udp_sockSrv = socket(AF_INET, SOCK_DGRAM, 0);//创建UPD套接字
	SOCKADDR_IN udp_addrSrv;
	udp_addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	udp_addrSrv.sin_family = AF_INET;
	udp_addrSrv.sin_port = htons(1234);
	int len = sizeof(udp_addrSrv);

	bind(udp_sockSrv, (SOCKADDR*)&udp_addrSrv, sizeof(SOCKADDR));
	printf("UDP server starting ...  \n");

	FD_SET(udp_sockSrv, &fdSocket);
	

	while (TRUE)
	{

		//通过select等待数据到达事件，如果有事件发生，select函数移除fdRead集合中没有未决I/O操作的套接字句柄，然后返回
		fdRead = fdSocket;
		iResult = select(0, &fdRead, NULL, NULL, NULL);
		printf("iResult = %d\n", iResult);

		if (iResult >0)
		{
			//有网络事件发生
			//确定有哪些套接字有未决的I/O，并进一步处理这些I/O
			for (int i = 0; i<(int)fdSocket.fd_count; i++)
			{
				if (FD_ISSET(fdSocket.fd_array[i], &fdRead))
				{
					if (fdSocket.fd_array[i] == ServerSocket)
					{
						if (fdSocket.fd_count < FD_SETSIZE)
						{
							//同时复用的套接字数量不能大于FD_SETSIZE
							//有新的连接请求
							AcceptSocket = accept(ServerSocket, (sockaddr FAR*)&addrClient, &addrClientlen);//有请求连接的TCP客户端
							if (AcceptSocket == INVALID_SOCKET)
							{
								printf("accept failed !\n");
								closesocket(ServerSocket);
								WSACleanup();
								return 1;
							}
							//增加新的连接套接字进行复用等待
							FD_SET(AcceptSocket, &fdSocket);
							printf("接收到新的连接：%s\n", inet_ntoa(addrClient.sin_addr));
						}
						else
						{
							printf("连接个数超限!\n");
							continue;
						}
					}
						else if (fdSocket.fd_array[i] == udp_sockSrv)//接收到的连接是UDP套接字
					{
					iResult=recvfrom(fdSocket.fd_array[i], recvbuf, recvbuflen, 0, (SOCKADDR*)&udp_addrSrv, &len);
					if (iResult > 0)
					{
					//情况1：成功接收到数据
					printf("\nUDP　Bytes received: %d\n", iResult);
					//处理数据请求
					char sendBuf[100];
					sprintf(sendBuf, "echo:%s", recvbuf);
					//strcat_s(recvbuf, "--echo");
					sendto(fdSocket.fd_array[i], recvbuf, sizeof(recvbuf), 0, (SOCKADDR*)&udp_addrSrv, sizeof(SOCKADDR));//发送回射内容
					//....
					}
					else
					{
					//情况2：接收失败
					printf("UPD  recv failed with error: %d\n", WSAGetLastError());
					closesocket(fdSocket.fd_array[i]);
					FD_CLR(fdSocket.fd_array[i], &fdSocket);
					}

					}
					else
					{
						//有数据到达
						memset(recvbuf, 0, recvbuflen);
						iResult = recv(fdSocket.fd_array[i], recvbuf, recvbuflen, 0);//有来自TCP客户端的数据到达
						if (iResult > 0)
						{
							//情况1：成功接收到数据
							printf("\nTCP Bytes received: %d\n", iResult);
							//strcat_s(recvbuf, "--echo");
							send(fdSocket.fd_array[i], recvbuf, strlen(recvbuf) + 1, 0);//发送回射内容
								            
						}
						else if (iResult == 0)
						{
							//情况2：连接关闭
							printf("TCP Current Connection closing...\n");
							closesocket(fdSocket.fd_array[i]);
							FD_CLR(fdSocket.fd_array[i], &fdSocket);
						}
						else
						{
							//情况3：接收失败
							printf("recv failed with error: %d\n", WSAGetLastError());
							closesocket(fdSocket.fd_array[i]);
							FD_CLR(fdSocket.fd_array[i], &fdSocket);
						}
					}
				}
			}
		}
		else
		{
			printf("select failed with error: %d\n", WSAGetLastError());
			break;
		}
	}

	// cleanup 
	closesocket(ServerSocket);
	WSACleanup();
	return 0;
}
