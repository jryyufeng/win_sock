#include "stdafx.h"
#include <iostream>
#include <string>
#include <Winsock2.h>
#pragma comment(lib, "WS2_32")
using namespace std;

int main()
{
    //����socket�⺯��
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD( 1, 1 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        cout<<"��ʼ��Socketʧ�ܣ�"<<endl;
        return 0;
    }
    if ( LOBYTE( wsaData.wVersion ) != 1 ||
            HIBYTE( wsaData.wVersion ) != 1 )
    {
        cout<<"��ʼ��Socketʧ�ܣ�"<<endl;
        WSACleanup();
        return 0;
    }
    //�����׽���
    SOCKET sockClient = socket(AF_INET,SOCK_STREAM,0) ;

    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");

    addrSrv.sin_family = AF_INET;

    addrSrv.sin_port = htons(1234);

    if(connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR))!=0)
    {
        cout<<"���ӷ�����ʧ�ܣ�"<<endl;;
        WSACleanup();
        return 0;
    }
	char sendBuf[100];


//�˿ͻ������ڽ��ձ䳤����ʱǰ��λ���巢���ַ��ĳ��ȣ�Ĭ�ϳ��ȼ���Ϊ ǰ��λ/256 
//���ն�����߳����⣬����һ�η�����ÿ�η�����Ϻ�����ٷ���һ�β��ܽ�����һ�η��͵���Ϣ 
	while(1)
	{
		cout<<"input��";
		cin>>sendBuf;
		cout<<"aaaaaaaa";
		send(sockClient,sendBuf,0,0);
		cout<<"Receving..."<<endl;
		char recvBuf[300];
    	recv(sockClient,recvBuf,300,0);
    	printf("%s.\n",recvBuf);
	}
	

    closesocket(sockClient);
    WSACleanup();
    return 0;
}
