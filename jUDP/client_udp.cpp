#include<iostream>
#include <stdio.h>
#include<stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") 
using namespace std;

int main()
{
    WORD socketVersion = MAKEWORD(2,2);
    WSADATA wsaData; 
	char sendData[10] = "aaaaabbbb";
	char recvData[10] = "";
	char num[5];
	int number = 0;
	int count = 1;
    if(WSAStartup(socketVersion, &wsaData) != 0)
    {
        return 0;
    }
    SOCKET sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(1234);
    sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    int len = sizeof(sin);
    // ����mem��Сֵ 
	memset(num, 0, sizeof(num));
	printf("ͨ��˫��Э�̱��ĸ���\n");
	cin>>num;
   	sendto(sclient, num, sizeof(num), 0, (sockaddr *)&sin, len);
	number=atoi(num);
	getchar();
	cout<<"�س�����"<<endl; 
	getchar();
    while(number --)
	{
		printf("���͵�%d�����ݰ�\n",count);
		sendto(sclient, sendData, sizeof(sendData), 0, (sockaddr *)&sin, len);
		count ++;
  
	}
	printf("������ɣ��˳�\n");

    closesocket(sclient);
    WSACleanup();
	system("pause");
    return 0;
}
