#include "stdafx.h"
#include <iostream>
#include <Winsock2.h>
#include "stdio.h"
#include <time.h>
#pragma comment(lib, "WS2_32")

using namespace std;
//DWORD���޷��ŵ�,�൱��unsigned long
unsigned long WINAPI IOthread(LPVOID lpParam)
{
	time_t t;
	char sendBuf[100];
	char recvBuf[100];
	SOCKET client_socket=(SOCKET)(LPVOID)lpParam;
	
	//��ȡϵͳʱ��
	t=time(0); 
	strftime(sendBuf,sizeof(sendBuf),"%Y %m %d %X %A",localtime(&t));//������ʱ���ʽ��Ϊ������Ҫ�ĸ�ʽ
	//0��ʾ������ ��ʽ�������ݣ�
	//recv(client_socket,recvBuf,100,0);//����ֵָʾ�˽������ݵ��ܳ��� 
	send(client_socket,sendBuf,strlen(sendBuf)+1,0);
	closesocket(client_socket);
	return 0;
	
}

int main()
{
	//����socket�⺯��
	WORD wVersionRequested;
	WSADATA wsadata;
	int err;
	//MAKEWORD(x,y)(���У�x�Ǹ�λ�ֽڣ�y�ǵ� λ�ֽ�)����ػ��wVersionRequested����ȷֵ  
	//��λ�ֽ�ָ������Ҫ�� Winsock��ĸ��汾������λ�ֽ��������汾
	//MAKEWORE(1,1)��MAKEWORD(2,2)���������ڣ�ǰ��ֻ��һ�ν���һ�Σ��������Ϸ��ͣ��������ܡ� 
	wVersionRequested = MAKEWORD( 1, 1 );//1��1�ϳ�һ������ 
	//�汾Э�̣�������Դ 
	err = WSAStartup( wVersionRequested, &wsadata );
	if ( err != 0 )
    {
        cout<<"��ʼ��Socketʧ�ܣ�"<<endl;
        return 0;
    }
    if ( LOBYTE( wsadata.wVersion ) != 1 ||
            HIBYTE( wsadata.wVersion ) != 1 )
    {
        cout<<"��ʼ��Socketʧ�ܣ�"<<endl;
        WSACleanup();
        return 0;
    }
    //�����׽���
	SOCKET sock_srv=socket(AF_INET,SOCK_STREAM,0);
	//��ַ�ṹ�� 
	SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(1234); 
    //�󶨶˿�
	bind(sock_srv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	//�����ͻ���
	listen(sock_srv,5); 
	//����ӿͻ��˽��ܵĵ�ַ��Ϣ
    SOCKADDR_IN addrClient ;
    int len = sizeof(SOCKADDR);
    HANDLE  hThread;
    cout<<"�ȴ��ͻ�����:"<<endl;
    SOCKET AcceptSocket;
    while(true){
    	AcceptSocket = SOCKET_ERROR;
        AcceptSocket = accept(sock_srv,(SOCKADDR*)&addrClient,&len);
        printf("welcome %s \n",inet_ntoa(addrClient.sin_addr));
         hThread=CreateThread(NULL,0,IOthread,(LPVOID)AcceptSocket,0,NULL);
        if(hThread==NULL)
        {
            cout<<"�����߳�ʧ�ܣ�"<<endl;
        }
        else
        {
            cout<<"�����̴߳����ɹ���"<<endl;
        }

	}
	//�ر��׽��� 
	closesocket(sock_srv);
    WSACleanup();
    return 0;
	 
}













