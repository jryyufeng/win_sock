// EchoTCPServer-select.cpp : �������̨Ӧ�ó������ڵ㡣
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

// ���ӵ�winsock2��Ӧ��lib�ļ��� Ws2_32.lib 
#pragma comment (lib, "Ws2_32.lib") 

#define DEFAULT_BUFLEN 512    //Ĭ�ϻ���������Ϊ512
#define DEFAULT_PORT 27015    //Ĭ�Ϸ������˿ں�Ϊ27015
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

	// ��ʼ�� Winsock 
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// �������ڼ������׽���   
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	// Ϊ�׽��ְ󶨵�ַ�Ͷ˿ں�
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;
	//addrServ.sin_port = htons(DEFAULT_PORT);		// �����˿�ΪDEFAULT_PORT
	addrServ.sin_port = htons(1234);
	addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	iResult = bind(ServerSocket, (const struct sockaddr*)&addrServ, sizeof(SOCKADDR_IN));
	// �����׽���   
	iResult = listen(ServerSocket, SOMAXCONN);

	//=====================================================================
	SOCKET udp_sockSrv = socket(AF_INET, SOCK_DGRAM, 0);//����UPD�׽���
	SOCKADDR_IN udp_addrSrv;
	udp_addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	udp_addrSrv.sin_family = AF_INET;
	udp_addrSrv.sin_port = htons(1234);
	int len = sizeof(udp_addrSrv);

	bind(udp_sockSrv, (SOCKADDR*)&udp_addrSrv, sizeof(SOCKADDR));

	fd_set fdRead, fdSocket;
	FD_ZERO(&fdSocket);
	FD_SET(ServerSocket, &fdSocket);
	FD_SET(udp_sockSrv, &fdSocket);
	

	while (TRUE)
	{

		//ͨ��select�ȴ����ݵ����¼���������¼�������select�����Ƴ�fdRead������û��δ��I/O�������׽��־����Ȼ�󷵻�
		fdRead = fdSocket;
		iResult = select(0, &fdRead, NULL, NULL, NULL);

		if (iResult >0)
		{
			//�������¼�����
			//ȷ������Щ�׽�����δ����I/O������һ��������ЩI/O
			for (int i = 0; i<(int)fdSocket.fd_count; i++)
			{
				if (FD_ISSET(fdSocket.fd_array[i], &fdRead))
				{
					if (fdSocket.fd_array[i] == ServerSocket)
					{						
							//ͬʱ���õ��׽����������ܴ���FD_SETSIZE
							//���µ���������
							AcceptSocket = accept(ServerSocket, (sockaddr FAR*)&addrClient, &addrClientlen);//���������ӵ�TCP�ͻ���
							//�����µ������׽��ֽ��и��õȴ�
							FD_SET(AcceptSocket, &fdSocket);						
					}
					
					else if (fdSocket.fd_array[i] == udp_sockSrv)//���յ���������UDP�׽���
					{
						iResult=recvfrom(fdSocket.fd_array[i], recvbuf, recvbuflen, 0, (SOCKADDR*)&udp_addrSrv, &len);
						//������������
						char sendBuf[100];
						sprintf(sendBuf, "echo:%s", recvbuf);
						//strcat_s(recvbuf, "--echo");
						sendto(fdSocket.fd_array[i], recvbuf, sizeof(recvbuf), 0, (SOCKADDR*)&udp_addrSrv, sizeof(SOCKADDR));//���ͻ�������
						}
					}
					
					else
					{
						//�����ݵ���
						memset(recvbuf, 0, recvbuflen);
						iResult = recv(fdSocket.fd_array[i], recvbuf, recvbuflen, 0);//������TCP�ͻ��˵����ݵ���
						//���1���ɹ����յ�����
						printf("\nTCP Bytes received: %d\n", iResult);
						//strcat_s(recvbuf, "--echo");
						send(fdSocket.fd_array[i], recvbuf, strlen(recvbuf) + 1, 0);//���ͻ�������

					
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
