#include <Winsock2.h>
#include <stdio.h>
#include<stdlib.h>
#pragma comment(lib, "ws2_32.lib")
#include<iostream>
using namespace std;

int main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 1, 1 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		return -1;
	}


	if ( LOBYTE( wsaData.wVersion ) != 1 ||
        HIBYTE( wsaData.wVersion ) != 1 ) {
		WSACleanup( );
		return -1;
	}
	SOCKET sockSrv=socket(AF_INET,SOCK_DGRAM,0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(1234);

	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	char recvBuf[100];
	char sendBuf[100];
	char tempBuf[200];

	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);

	while(1)
	{
		//多了 int len, int flags) 来源IP＋来源端口号 和 来源长度 
		recvfrom(sockSrv,recvBuf,100,0,(SOCKADDR*)&addrClient,&len);
		cout<<recvBuf<<"-------------------\n";
		sprintf(tempBuf,"%s say : %s",inet_ntoa(addrClient.sin_addr),recvBuf);
		printf("%s\n",tempBuf);
		
		sprintf(sendBuf,"echo:%s",recvBuf);
		
		sendto(sockSrv,sendBuf,strlen(sendBuf)+1,0,(SOCKADDR*)&addrClient,len);

		if (strcmp("q",recvBuf)==0)
		{
			//sendto(sockSrv, "q", strlen("q") + 1, 0, (SOCKADDR*)&addrClient, len);
			printf("Chat end!\n");
			break;
		}
	}
	closesocket(sockSrv);
	WSACleanup();
	system("pause");
	return 0;
}

