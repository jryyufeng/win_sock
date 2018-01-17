#include<iostream>
#include <stdio.h>
#include<stdlib.h>
#include <winsock2.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib") 
using namespace std;

int main()
{
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2,2);
	char recvData[10]= ""; 
	char number[5];
	int size = 0;
	int receive = 0;
	int tt = 0;
    if(WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }

    SOCKET serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    if(serSocket == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(1234);
    serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    if(bind(serSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
        printf("bind error !");
        closesocket(serSocket);
        return 0;
    }
    
    sockaddr_in remoteAddr;
	
    int nAddrLen = sizeof(remoteAddr); 
	memset(number, 0, sizeof(number));
	int ret = recvfrom(serSocket, number, sizeof(number), 0, (sockaddr *)&remoteAddr, &nAddrLen);
	printf("%d\n",ret);
	if (ret > 0)
        {
            printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
                        
        }
	printf("客户端发包数目为:%s\n",number);
	int n = atoi(number);
	printf("设置服务端的接收缓存大小(byte)\n");
	scanf("%d",&size);
	//设置缓存区大小 
	setsockopt(serSocket, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size));
	while (1)
    {     
       int iResult= recvfrom(serSocket, recvData, sizeof(recvData), 0, (sockaddr *)&remoteAddr, &nAddrLen);
		//printf("Received ,,,\n");
		tt++;	//t用来计量实际收到的包的个数
		printf("iResult=%d\n", iResult);
		if (iResult> 0)
		{
			receive++;
			//printf("%s\n", recvData);
			printf("收到 %d 个包...\n", tt);
		}
		else
		{
			printf("丢失第 %d 个包...\n", tt);
		}
		Sleep(100);
		if(tt == n)
			break;
    }
    
	printf("recv = %d\n", receive);
	float t = 1-((float)receive/(float)n);
	printf("losing rate:%f\n",t);
	closesocket(serSocket); 
    WSACleanup();
	system("pause");
    return 0;
}
