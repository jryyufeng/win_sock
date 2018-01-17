#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#pragma comment(lib,"ws2_32.lib")
#include<Winsock2.h>

using namespace std;
DWORD WINAPI IOThread(LPVOID lpParam);

struct RECVADDR{
	SOCKET sock;
	SOCKADDR_IN addrClient;
	char buf[100];
};

int main()
{
	WORD wV;
	WSADATA	wsadata;
	int err;
	wV=MAKEWORD(2,2);
	err=WSAStartup(wV,&wsadata);
	if(err!=0){
		cout<<"初始化失败"<<endl;
		return -1;
	}	 
	if(LOBYTE(wsadata.wVersion)!=2||HIBYTE(wsadata.wVersion)!=2){
		cout<<"初始化失败"<<endl;
		WSACleanup();
		return -1;
	}
	//创建套接字并初始化
	SOCKET sock_server=socket(AF_INET,SOCK_DGRAM,0);
	SOCKADDR_IN addr_server;
	addr_server.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addr_server.sin_family=AF_INET;
	addr_server.sin_port=htons(1234);
	//绑定socket与地址
	bind(sock_server,(SOCKADDR*)&addr_server,sizeof(SOCKADDR));
	
	// 等待客户端连接
	SOCKADDR_IN addr_client;
	int len=sizeof(SOCKADDR);
	int k=0;
	
	RECVADDR udp_socket;
	udp_socket.sock=sock_server;
	while(true){
		recvfrom(udp_socket.sock,udp_socket.buf,100,0,(SOCKADDR*)&addr_client,&len);
		cout<<"client:"<<udp_socket.buf<<endl;
		udp_socket.addrClient=addr_client;
		HANDLE hThread = CreateThread(NULL, 0, IOThread, (LPVOID)&(udp_socket), 0, NULL);
		CloseHandle(hThread);
		if(strcmp("q",udp_socket.buf)==0)
		{
			break;
		}
	}
	Sleep(500);
	closesocket(sock_server);
	WSACleanup();
	system("pasue");
	return 0; 
}
DWORD WINAPI IOThread(LPVOID lpParam){
	cout<<"创建线程成功"<<endl;
	int temp_port=rand()%1000+1234;
	SOCKET temp_sock_server=socket(AF_INET,SOCK_DGRAM,0);
	SOCKADDR_IN temp_addr_server;
	temp_addr_server.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	temp_addr_server.sin_family=AF_INET;
	temp_addr_server.sin_port=htons(temp_port);
	//绑定
	bind(temp_sock_server,(SOCKADDR*)&temp_addr_server,sizeof(SOCKADDR));
	cout<<"新端口为："<<temp_port<<endl;
	
	//处理信息
	char recvBuf[100]; 
	char sendBuf[100];
	int len=sizeof(SOCKADDR);
	strcpy(recvBuf,((RECVADDR*)(lpParam))->buf);
	while(true){
		sprintf(sendBuf, "echo:%s", recvBuf);
		if (strcmp("q", recvBuf )!= 0)
		{	//向服务器发送消息
			((RECVADDR*)(lpParam))->sock=temp_sock_server;
			//sprintf(sendBuf, "echo:%s", ((RECVADDR*)(lpParameter))->buf);
			sendto(((RECVADDR*)(lpParam))->sock, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR*)&(((RECVADDR*)(lpParam))->addrClient), sizeof(SOCKADDR));
			
		}
		//断开链接 
		else{
			sendto(temp_sock_server, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR*)&((RECVADDR*)(lpParam))->addrClient, sizeof(SOCKADDR));
			printf("sever：%s\n", ((RECVADDR*)(lpParam))->buf);
			printf("chat end!\n");
			break;
		}
		recvfrom(temp_sock_server,recvBuf,sizeof(recvBuf)+1,0, (SOCKADDR*)&((RECVADDR*)(lpParam))->addrClient, &len);
		cout<<"client inform:"<<recvBuf<<endl;
	}
	closesocket(temp_sock_server);
	return 0; 
}










