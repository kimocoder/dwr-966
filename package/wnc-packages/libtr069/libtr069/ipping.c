#include <stdio.h>
#include <windows.h>
#include <winsock.h>
#pragma comment(lib,"Ws2_32.lib")
#include "Common.h"
#include "logger.h"
#define SEND_SIZE 32 
#define PACKET_SIZE 4096
#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0

struct icmp
{
	unsigned char icmp_type;
	unsigned char icmp_code;
	unsigned short icmp_cksum;
	unsigned short icmp_id;
	unsigned short icmp_seq;
	unsigned long icmp_data; 
};

struct ip
{
	unsigned char ip_hl:4;       
	unsigned char ip_v:4;        
	unsigned char ip_tos;           
	unsigned short ip_len;         
	unsigned short ip_id;          
	unsigned short ip_off;         
	unsigned char ip_ttl;           
	unsigned char ip_p;          
	unsigned short ip_sum;         
	unsigned long ip_src;
	unsigned long ip_dst;  
};

static int DataId = 1234;
unsigned char sendpacket[PACKET_SIZE];
unsigned char recvpacket[PACKET_SIZE];
struct sockaddr_in dest_addr;
struct sockaddr_in from_addr;



//this algorithm is referenced from other's 
static unsigned short cal_chksum(unsigned short *addr,int len)
{       
	int nleft=len;
	int sum=0;
	unsigned short *w=addr;
	unsigned short answer=0;
 
    while (nleft>1)
    {       
		sum += *w++;
		nleft -= 2;
    }
    if (nleft == 1)
    {       
		*(unsigned char *)(&answer)=*(unsigned char *)w;
		sum+=answer;
    }
	sum = (sum>>16)+(sum&0xffff);
	sum += (sum>>16);
	answer =~ sum;
	return answer;
}

static int pack(int pack_no)
{       
	int packsize;
	struct icmp *icmp;

	packsize = 8+SEND_SIZE;
	icmp = (struct icmp*)sendpacket;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_cksum = 0;
	icmp->icmp_seq = pack_no;
	icmp->icmp_id = DataId;
	icmp->icmp_data = GetTickCount();
	icmp->icmp_cksum = cal_chksum( (unsigned short *)icmp,packsize); /*校验算法*/
	return packsize;
}

static int unpack(unsigned char *buf,int len)
{      
	struct ip *ip;
	struct icmp *icmp;
	double rtt;
	int iphdrlen;

    ip = (struct ip *)buf;
    iphdrlen = ip->ip_hl*4;    
    icmp = (struct icmp *)(buf+iphdrlen);           
    if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == DataId))
    { 
		len=len-iphdrlen-8;   
		rtt=GetTickCount()-icmp->icmp_data;
		LOG(LOG_DEBUG, "Reply from %s: bytes=%d time=%.0fms TTL=%d icmp_seq=%u\n",             
		inet_ntoa(from_addr.sin_addr),
		len,
		rtt,
		ip->ip_ttl,
		icmp->icmp_seq);
		return 1;
    }
	return 0;
}


int ping(char* szHost, unsigned int timeout, unsigned int NumberOfRepetitions, unsigned int DataBlockSize, unsigned int *SuccessCount, unsigned int *FailureCount, unsigned int *AverageResponseTime, unsigned int *MinimumResponseTime, unsigned int *MaximumResponseTime)
{       
	struct hostent *host;
	WSADATA wsaData;
	unsigned int i;
	int n,fromlen;
	int success = 0;
	int packetsize;
	int sockfd;
	int iReturn = _SUCCESS;

	if (WSAStartup(0x1010,&wsaData)!=0)
	{
		LOG(LOG_DEBUG, "wsastartup error\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
	if ((sockfd=socket(AF_INET, SOCK_RAW, IPPROTO_ICMP))<0)
    {   
		LOG(LOG_DEBUG, "socket error %d\n",  WSAGetLastError());
		iReturn = _ERROR;
		goto FUNC_EXIT;
    }

	if(setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(timeout))<0)  
		LOG(LOG_DEBUG, "failed to set recv timeout: %d\n",WSAGetLastError());
	if(setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,sizeof(timeout))<0)
		LOG(LOG_DEBUG, "failed to set send timeout: %d\n",WSAGetLastError());       

    memset(&dest_addr,0,sizeof(dest_addr));
    dest_addr.sin_family=AF_INET; 
    if(host=gethostbyname(szHost) )
    {
		memcpy( (char *)&dest_addr.sin_addr,host->h_addr,host->h_length);
		//resolve address to hostname
		//if(host=gethostbyaddr(host->h_addr,4,PF_INET))
		//	par_host=host->h_name;
	}
    else if( dest_addr.sin_addr.s_addr=inet_addr(szHost)==INADDR_NONE)
	{
		LOG(LOG_DEBUG, "Unkown host %s\n",szHost);
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
      
    LOG(LOG_DEBUG, "Pinging %s [%s]: with %d bytes of data:\n\n", szHost, inet_ntoa(dest_addr.sin_addr), SEND_SIZE);                    
	for(i = 0; i < NumberOfRepetitions; i++)                        
	{
		//---send
		packetsize=pack(i); 
		if (sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
			LOG(LOG_DEBUG, "Destination host unreachable.\n");
		//---
		//---recv
		fromlen=sizeof(from_addr);
		if ((n=recvfrom(sockfd,recvpacket,sizeof(recvpacket),0,(struct sockaddr *)&from_addr,&fromlen)) >=0)
		{
			success=unpack(recvpacket,n);
			*SuccessCount++;
		}
		else if (WSAGetLastError() == WSAETIMEDOUT) 
		{
			LOG(LOG_DEBUG, "Request timed out.\n");
			*FailureCount++;
		}
		//---
		Sleep(1000);
	}
FUNC_EXIT:
	return iReturn;
}
