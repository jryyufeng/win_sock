#include "stdafx.h"
#include <iostream>
#include <string>
#include <Winsock2.h>
#pragma comment(lib, "WS2_32")

using namespace std;
int recvn(SOCKET s,char *recvbuf, unsigned int fixedlen)
{
	int iResult;
	int cnt;
	cnt=fixedlen;
	cout<<"fixedlen="<<cnt<<endl;
	while(cnt>0)
	{
		iResult=recv(s,recvbuf,cnt,0);
		cout<<"iresutl="<<iResult<<endl;
		if(iResult<0)
		{
			printf("11111error:%d\n",WSAGetLastError());
			return -1;
		}
		if(iResult==0)
		{
			printf("closed\n");
			return fixedlen-cnt;
		}

		recvbuf+=iResult;
		cnt-=iResult;
		cout<<"cnt="<<cnt<<endl;

	}
	return fixedlen;
}
int recvvl(SOCKET s,char *recvbuf, unsigned int recvbuflen)
{
	int iResult;
	char length[sizeof(unsigned int)];
	unsigned int reclen;

	iResult=recvn(s,length,sizeof(unsigned int));	//接收到消息长度
	cout<<length<<endl;
	reclen=atoi(length);
	
	if(iResult!=(sizeof(unsigned int)))
	{
		if(iResult==-1)
		{
			printf("error:%d\n",WSAGetLastError());
			return -1;
		}
		else
		{
			printf("closed\n");
			return 0;
		}
	}
	reclen/=256;//定义接收长度 
	cout<<"长度是 "<<reclen<<endl;
	cout<<"缓冲区空间是 "<<recvbuflen<<endl;

	if(reclen>recvbuflen)
	{
		while(reclen>0)
		{
			iResult=recvn(s,recvbuf,recvbuflen);//根据消息长度进行定长数据的接收
			if(iResult!=recvbuflen)
			{
				if(iResult==-1)
				{
					printf("22222error:%d\n",WSAGetLastError());
					return -1;
				}
				else
				{
					printf("closed\n");
					return 0;
				}
			}
			reclen-=recvbuflen;
			if(reclen<recvbuflen)
			{
				reclen=recvbuflen;
			}
		}
		cout<<"超出缓存"<<endl;
		return -1;
	}

	iResult=recvn(s,recvbuf,reclen);
	if(iResult!=reclen)
	{
		if(iResult==-1)
		{
			printf("33333error:%d\n",WSAGetLastError());
			return -1;
		}
		else
		{
			printf("closed\n");
			return 0;
		}
	}
	return reclen;

}


int main()
{
	WORD wV;
	WSADATA wsa;
	int err;
	wV=MAKEWORD(1,1);
	err=WSAStartup(wV,&wsa);
	if ( err != 0 )
    {
        cout<<"初始化Socket失败！"<<endl;
        return 0;
    }
    if ( LOBYTE( wsa.wVersion ) != 1 ||
            HIBYTE( wsa.wVersion ) != 1 )
    {
        cout<<"初始化Socket失败！"<<endl;
        WSACleanup();
        return 0;
    }
    //创建套接字
	SOCKET sock_server=socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN addr_s;
	addr_s.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addr_s.sin_family = AF_INET;
    addr_s.sin_port = htons(1234);
	//绑定端口
	bind(sock_server,(SOCKADDR*)&addr_s,sizeof(SOCKADDR));
	//监听客户端
	listen(sock_server,5);
	//定义从客户端接收的地址信息
	SOCKADDR_IN addr_c;
	int len=sizeof(SOCKADDR);
	cout<<"等待连接"<<endl;
	SOCKET accept_s;
	char sendBuf[100];
	char recvBuf[100];
	int as=100;
	while(true){
		accept_s=SOCKET_ERROR;
		accept_s=accept(sock_server,(SOCKADDR*)&addr_c,&len);
		printf("welcome %s \n",inet_ntoa(addr_c.sin_addr));
		SOCKET sock_client=(SOCKET)(LPVOID)accept_s;
		while(true){
			recvvl(sock_client,recvBuf,40000000);
			//recv(sock_client,recvBuf,as,0);
			cout<<as<<endl;
			sprintf(sendBuf,"echo:%s",recvBuf);
			send(sock_client,sendBuf,strlen(sendBuf)+1,0);
		}
		closesocket(sock_client);
		
	} 
	closesocket(sock_server);
	WSACleanup();
	return 0; 
	 
}












