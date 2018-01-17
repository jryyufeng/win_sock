#include "stdafx.h"
#include<iostream>
#include<string>
#include<Winsock2.h>

#pragma comment(lib,"WS2_32")
using namespace std;
int recvn(SOCKET s, char * recvbuf, unsigned int fixedlen) 
{     
	int iResult;    
	//存储单次recv操作的返回值     
	int cnt;   
 	//用于统计相对于固定长度，剩余多少字节尚未接收     
 	cnt = fixedlen;  
	char* pp=recvbuf;   
 	while ( cnt > 0 ) 
 	{         
 		iResult = recv(s, pp, 5, 0); 
		cout<<recvbuf<<"=========接收数据"<<endl;        
 		if ( iResult < 0 )
 		{             
 			//数据接收出现错误，返回失败             
 			printf("接收发生错误: %d\n", WSAGetLastError());    
    		return -1;         
		}   
		if ( iResult == 0 )
		{        
			//对方关闭连接，返回已接收到的小于fixedlen的字节数             
			printf("连接关闭\n");             
			return fixedlen - cnt;         
		}         
			printf("接收到的字节数(打印值为recv函数中指定的值): %d\n", iResult);         
			//接收缓存指针向后移动  
			pp +=iResult;         
			//更新cnt值         
			cnt -=iResult;  
			cout<<cnt<<"------------cnt"<<endl;            
	}   
	  
	return cnt; 
} 

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

    cout<<"等待客户连接:"<<endl;
    SOCKET AcceptSocket;

        AcceptSocket = SOCKET_ERROR;
        AcceptSocket = accept(sockSrv,(SOCKADDR*)&addrClient,&len);
        printf("welcome %s \n",inet_ntoa(addrClient.sin_addr));

        char sendBuf[100];
        
    	char recvBuf[100];
    	SOCKET  ClientSocket=(SOCKET)(LPVOID)AcceptSocket;
    	while(1)
    	{
        	recvn(ClientSocket,recvBuf,20);
        		if(strcmp(recvBuf,"bye")==0)
			{
				cout<<"Bye..."<<endl;
				sprintf(sendBuf,"exit");
				send(ClientSocket,sendBuf,strlen(sendBuf)+1,0);
				break;
			}

        	//recv(ClientSocket,recvBuf,20,0);
        	sprintf(sendBuf,"echo:%s",recvBuf);
        	send(ClientSocket,sendBuf,strlen(sendBuf)+1,0);
    	}
    	closesocket(ClientSocket);

    closesocket(sockSrv);
    WSACleanup();
    return 0;
}

























