#include "initsock.h"
#include "protoinfo.h"
#include <iostream>
#include "ping.h"
#include <stdio.h>
#include <mstcpip.h>
using namespace std;

u_short in_cksum(u_short *addr, int len)
{
	register int nleft = len;
	register u_short *w = addr;
	register u_short answer;
	register int sum = 0;
	while( nleft > 1 )  {
		sum += *w++;
		nleft -= 2;
	}

	if( nleft == 1 ) {
		u_short	u = 0;

		*(u_char *)(&u) = *(u_char *)w ;
		sum += u;
	}

	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return (answer);
}

void ReportError(LPCSTR pWhere)
{
	fprintf(stderr,"\n%s error: %d\n",
		WSAGetLastError());
}
int sendechorequest(SOCKET s,LPSOCKADDR_IN lpstToAddr) 
{
	static ECHOREQUEST echoReq;
	static int nId = 1;
	static int nSeq = 1;
	int nRet;

	// 构造请求 
	echoReq.icmpHdr.Type		= ICMP_ECHOREQ;
	echoReq.icmpHdr.Code		= 0;
	echoReq.icmpHdr.Checksum	= 0;
	echoReq.icmpHdr.ID			= nId++;
	echoReq.icmpHdr.Seq			= nSeq++;

	// 填充数据
	for (nRet = 0; nRet < REQ_DATASIZE; nRet++)
		echoReq.cData[nRet] = ' '+nRet;

	// 获取当前时钟并填充 
	echoReq.dwTime = GetTickCount();

	// 计算校验和 
	echoReq.icmpHdr.Checksum = in_cksum((u_short *)&echoReq, sizeof(ECHOREQUEST));

	// 发送请求 								  
	nRet = sendto(s,				//套接字 
				 (LPSTR)&echoReq,		//缓冲区 
				 sizeof(ECHOREQUEST),//缓冲区大小 
				 0,							//发送标识 
				 (LPSOCKADDR)lpstToAddr,//目标地址 
				 sizeof(SOCKADDR_IN));  //地址长度 

	if (nRet == SOCKET_ERROR) 
		ReportError("sendto()");
	return (nRet);
}

DWORD recvechoreply(SOCKET s, LPSOCKADDR_IN lpsaFrom, u_char *pTTL) 
{
	ECHOREPLY echoReply;
	int nRet;
	int nAddrLen = sizeof(struct sockaddr_in);

	// 接收回应	
	nRet = recvfrom(s,					// socket
					(LPSTR)&echoReply,	// buffer
					sizeof(ECHOREPLY),	// size of buffer
					0,					// flags
					(LPSOCKADDR)lpsaFrom,	// From address
					&nAddrLen);			// pointer to address len

	// Check return value
	if (nRet == SOCKET_ERROR) 
		ReportError("recvfrom()");

	// return time sent and IP TTL
	*pTTL = echoReply.ipHdr.TTL;
	return(echoReply.echoRequest.dwTime);   		
}


int waitforfchoreply(SOCKET s)
{
	struct timeval Timeout;
	fd_set readfds;

	readfds.fd_count = 1;
	readfds.fd_array[0] = s;
	Timeout.tv_sec = 5;
    Timeout.tv_usec = 0;

	return(select(1, &readfds, NULL, NULL, &Timeout));
}
void ping(char* pstrhost)
{
	SOCKET rawsocket;
	LPHOSTENT lphost;
	struct sockaddr_in sadest;//目的 
	struct sockaddr_in sasrc;//源 
	DWORD dwtimesent;
	DWORD dwelapsed;
	u_char cttl;
	int nloop;
	int nret;
	//创建套接字，制定类型为ICMP
	rawsocket=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
	if(rawsocket==SOCKET_ERROR){
		ReportError("socket()");
		return ;
	} 
	//根据用户指定的地址获取ip
	lphost=gethostbyname(pstrhost);
	if(lphost==NULL){
		cout<<"输入网址有误"<<endl;
		return ;
	} 
	//填充套接字的目的断电地址
	sadest.sin_addr.s_addr=*((u_long FAR*)(lphost->h_addr));
	sadest.sin_family=AF_INET;
	sadest.sin_port=0;
	
	printf("\nPinging %s [%s] with %d bytes of data:\n",pstrhost,inet_ntoa(sadest.sin_addr),REQ_DATASIZE); 
	
	for(nloop=0;nloop<4;nloop++){
		//发送icmp请求
		sendechorequest(rawsocket,&sadest);
		//wait
		nret=waitforfchoreply(rawsocket);
		if(nret==SOCKET_ERROR){
			ReportError("socket()");
			return ;
		}
		if(!nret){
			cout<<"time out\n";
		} 
		
		dwtimesent=recvechoreply(rawsocket,&sasrc,&cttl);
		
		dwelapsed=GetTickCount()-dwtimesent;
		
		printf("\nReply from: %s: bytes=%d time=%ldms TTL=%d", 
            inet_ntoa(sasrc.sin_addr), 
			REQ_DATASIZE,
            dwelapsed,
            cttl);
	}
	cout<<endl;
	nret=closesocket(rawsocket);
	if(nret==SOCKET_ERROR){
			ReportError("socket()");
			return ;
		}
}


int main()
{
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(1,1);
    int nRet;
    nRet = WSAStartup(wVersionRequested, &wsaData);
    if (nRet)
    {
		fprintf(stderr,"\nError initializing WinSock\n");
		return -1;
    }

	// Check version
	if (wsaData.wVersion != wVersionRequested)
	{
		fprintf(stderr,"\nWinSock version not supported\n");
		return -1;
	}
	char domain[100];
	//cout<<"输入待检测网址或ip"<<endl;
	printf("输入待检测网址或ip\n");
	// Go do the ping
	//cin>>domain;
	scanf("%s",&domain);
	ping(domain);
	// Free WinSock
    WSACleanup();
}























