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

	iResult=recvn(s,length,sizeof(unsigned int));	//���յ���Ϣ����
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
	reclen/=256;//������ճ��� 
	cout<<"������ "<<reclen<<endl;
	cout<<"�������ռ��� "<<recvbuflen<<endl;

	if(reclen>recvbuflen)
	{
		while(reclen>0)
		{
			iResult=recvn(s,recvbuf,recvbuflen);//������Ϣ���Ƚ��ж������ݵĽ���
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
		cout<<"��������"<<endl;
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
        cout<<"��ʼ��Socketʧ�ܣ�"<<endl;
        return 0;
    }
    if ( LOBYTE( wsa.wVersion ) != 1 ||
            HIBYTE( wsa.wVersion ) != 1 )
    {
        cout<<"��ʼ��Socketʧ�ܣ�"<<endl;
        WSACleanup();
        return 0;
    }
    //�����׽���
	SOCKET sock_server=socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN addr_s;
	addr_s.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addr_s.sin_family = AF_INET;
    addr_s.sin_port = htons(1234);
	//�󶨶˿�
	bind(sock_server,(SOCKADDR*)&addr_s,sizeof(SOCKADDR));
	//�����ͻ���
	listen(sock_server,5);
	//����ӿͻ��˽��յĵ�ַ��Ϣ
	SOCKADDR_IN addr_c;
	int len=sizeof(SOCKADDR);
	cout<<"�ȴ�����"<<endl;
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












