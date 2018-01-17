#include "stdafx.h"
#include <iostream>
#include <Winsock2.h>
#include "stdio.h"
#include <time.h>
#pragma comment(lib, "WS2_32")

using namespace std;
//DWORD是无符号的,相当于unsigned long
unsigned long WINAPI IOthread(LPVOID lpParam)
{
	time_t t;
	char sendBuf[100];
	char recvBuf[100];
	SOCKET client_socket=(SOCKET)(LPVOID)lpParam;
	
	//获取系统时间
	t=time(0); 
	strftime(sendBuf,sizeof(sendBuf),"%Y %m %d %X %A",localtime(&t));//函数将时间格式化为我们想要的格式
	//0表示按正常 方式发送数据；
	//recv(client_socket,recvBuf,100,0);//返回值指示了接收数据的总长度 
	send(client_socket,sendBuf,strlen(sendBuf)+1,0);
	closesocket(client_socket);
	return 0;
	
}

int main()
{
	//加载socket库函数
	WORD wVersionRequested;
	WSADATA wsadata;
	int err;
	//MAKEWORD(x,y)(其中，x是高位字节，y是低 位字节)方便地获得wVersionRequested的正确值  
	//高位字节指定所需要的 Winsock库的副版本，而低位字节则是主版本
	//MAKEWORE(1,1)和MAKEWORD(2,2)的区别在于，前者只能一次接收一次，不能马上发送，而后者能。 
	wVersionRequested = MAKEWORD( 1, 1 );//1和1合成一个新数 
	//版本协商，分配资源 
	err = WSAStartup( wVersionRequested, &wsadata );
	if ( err != 0 )
    {
        cout<<"初始化Socket失败！"<<endl;
        return 0;
    }
    if ( LOBYTE( wsadata.wVersion ) != 1 ||
            HIBYTE( wsadata.wVersion ) != 1 )
    {
        cout<<"初始化Socket失败！"<<endl;
        WSACleanup();
        return 0;
    }
    //创建套接字
	SOCKET sock_srv=socket(AF_INET,SOCK_STREAM,0);
	//地址结构体 
	SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(1234); 
    //绑定端口
	bind(sock_srv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	//监听客户端
	listen(sock_srv,5); 
	//定义从客户端接受的地址信息
    SOCKADDR_IN addrClient ;
    int len = sizeof(SOCKADDR);
    HANDLE  hThread;
    cout<<"等待客户连接:"<<endl;
    SOCKET AcceptSocket;
    while(true){
    	AcceptSocket = SOCKET_ERROR;
        AcceptSocket = accept(sock_srv,(SOCKADDR*)&addrClient,&len);
        printf("welcome %s \n",inet_ntoa(addrClient.sin_addr));
         hThread=CreateThread(NULL,0,IOthread,(LPVOID)AcceptSocket,0,NULL);
        if(hThread==NULL)
        {
            cout<<"创建线程失败！"<<endl;
        }
        else
        {
            cout<<"处理线程创建成功！"<<endl;
        }

	}
	//关闭套接字 
	closesocket(sock_srv);
    WSACleanup();
    return 0;
	 
}













