#include <iostream>
#include <Winsock2.h>
#include "stdio.h"
#pragma comment(lib, "WS2_32")
using namespace std;

DWORD WINAPI IOThread(LPVOID lpParam);

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
    SOCKET sockSrv = socket(AF_INET,SOCK_STREAM,0);

    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(1234);

    bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

    //监听客户端
    listen(sockSrv,5);

    //定义从客户端接受的地址信息
    SOCKADDR_IN addrClient ;
    int len = sizeof(SOCKADDR);

    HANDLE  hThread;
    cout<<"等待客户连接:"<<endl;
    SOCKET AcceptSocket;
    while(1)
    {
        AcceptSocket = SOCKET_ERROR;
        AcceptSocket = accept(sockSrv,(SOCKADDR*)&addrClient,&len);
        printf("welcome %s \n",inet_ntoa(addrClient.sin_addr));

        hThread=CreateThread(NULL,0,IOThread,(LPVOID)AcceptSocket,0,NULL);
        if(hThread==NULL)
        {
            cout<<"创建线程失败！"<<endl;
        }
        else
        {
            cout<<"创建处理线程成功！"<<endl;
        }
    }
    closesocket(sockSrv);
    WSACleanup();
    return 0;
}
DWORD WINAPI IOThread(LPVOID lpParam)
{
    char sendBuf[100];
    char recvBuf[100];
    SOCKET  ClientSocket=(SOCKET)(LPVOID)lpParam;
    while(1)
    {
        recv(ClientSocket,recvBuf,100,0);
        sprintf(sendBuf,"echo:%s",recvBuf);
        send(ClientSocket,sendBuf,strlen(sendBuf)+1,0);
    }
    closesocket(ClientSocket);
    return 0;
}
