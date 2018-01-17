#include "stdafx.h"
#include <iostream>
#include <string>
#include <Winsock2.h>
#pragma comment(lib, "WS2_32")
using namespace std;

int main()
{
    //加载socket库函数
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD( 1, 1 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        cout<<"初始化Socket失败！"<<endl;
        return 0;
    }
    if ( LOBYTE( wsaData.wVersion ) != 1 ||
            HIBYTE( wsaData.wVersion ) != 1 )
    {
        cout<<"初始化Socket失败！"<<endl;
        WSACleanup();
        return 0;
    }
    //创建套接字
    SOCKET sockClient = socket(AF_INET,SOCK_STREAM,0) ;

    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");

    addrSrv.sin_family = AF_INET;

    addrSrv.sin_port = htons(1234);

    if(connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR))!=0)
    {
        cout<<"连接服务器失败！"<<endl;;
        WSACleanup();
        return 0;
    }
	char sendBuf[100];


//此客户端用于接收变长数据时前四位定义发送字符的长度，默认长度计算为 前四位/256 
//接收定长与边长问题，除第一次发送外每次发送完毕后必须再发送一次才能接收上一次发送的信息 
	while(1)
	{
		cout<<"input：";
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
