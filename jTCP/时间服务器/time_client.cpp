#include "stdafx.h"
#include<iostream>
#include<Winsock2.h>
#pragma comment(lib,"WS2_32")

using namespace std;

int main()
{
	WORD wVersionRequested;
	WSADATA wsadata;
	int err;
	
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsadata);
	if(err!=0){
		cout<<"初始化sock失败";
		return 0;
	}
	if(LOBYTE( wsadata.wVersion ) != 1 || HIBYTE( wsadata.wVersion ) != 1)
	{
		cout<<"初始化sock失败";
		WSACleanup();
		return 0;
	 } 
	 //创建套接字
	 SOCKET sock_client=socket(AF_INET,SOCK_STREAM,0);
	 
	 SOCKADDR_IN add_srv;
	 
	 add_srv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	 add_srv.sin_family=AF_INET;
	 add_srv.sin_port=htons(1234);
	 
	 if(connect(sock_client,(SOCKADDR*)&add_srv,sizeof(SOCKADDR))!=0){
	 	cout<<"连接服务器失败！"<<endl;
	 	WSACleanup();
	 	return 0;
	 } 
	 char recvBuf[100];
	 string sendbuf;
     string recvbuf;
//	 while(true){
//	 	cin>>sendbuf;
//        send(sock_client,sendbuf.data(),sendbuf.length()+1,0);
//        recv(sock_client,recvBuf,100,0);
//	 	cout<<"当前时间为："<<recvBuf<<endl;
//	 	closesocket(sock_client);
//	 	WSACleanup();
//	 }
	    recv(sock_client,recvBuf,100,0);
	 	cout<<"当前时间为："<<recvBuf<<endl;
	 	closesocket(sock_client);
	 	WSACleanup();
	 return 0;
	 
}

















