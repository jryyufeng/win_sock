#include "stdafx.h"
#include<iostream>
#include<string>
#include<Winsock2.h>

#pragma comment(lib,"WS2_32")
using namespace std;
int recvn(SOCKET s, char * recvbuf, unsigned int fixedlen) 
{     
	int iResult;    
	//�洢����recv�����ķ���ֵ     
	int cnt;   
 	//����ͳ������ڹ̶����ȣ�ʣ������ֽ���δ����     
 	cnt = fixedlen;  
	char* pp=recvbuf;   
 	while ( cnt > 0 ) 
 	{         
 		iResult = recv(s, pp, 5, 0); 
		cout<<recvbuf<<"=========��������"<<endl;        
 		if ( iResult < 0 )
 		{             
 			//���ݽ��ճ��ִ��󣬷���ʧ��             
 			printf("���շ�������: %d\n", WSAGetLastError());    
    		return -1;         
		}   
		if ( iResult == 0 )
		{        
			//�Է��ر����ӣ������ѽ��յ���С��fixedlen���ֽ���             
			printf("���ӹر�\n");             
			return fixedlen - cnt;         
		}         
			printf("���յ����ֽ���(��ӡֵΪrecv������ָ����ֵ): %d\n", iResult);         
			//���ջ���ָ������ƶ�  
			pp +=iResult;         
			//����cntֵ         
			cnt -=iResult;  
			cout<<cnt<<"------------cnt"<<endl;            
	}   
	  
	return cnt; 
} 

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
    SOCKET sockSrv = socket(AF_INET,SOCK_STREAM,0);

    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(1234);

    bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

    //�����ͻ���
    listen(sockSrv,5);

    //����ӿͻ��˽��ܵĵ�ַ��Ϣ
    SOCKADDR_IN addrClient ;
    int len = sizeof(SOCKADDR);

    cout<<"�ȴ��ͻ�����:"<<endl;
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

























