#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <net/if.h>
int IP=0, ARP=0, RARP=0, TCP=0, UDP=0, ICMP=0, IGMP=0;

void count(unsigned char *buffer){
	struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));
	struct ether_header *eth = (struct ether_header *)buffer;
	switch (ntohs(eth->ether_type))
	{
		case 0x0800:
		++IP;
		break;

		case  0x0806:
		++ARP;
		break;

		case 0x8035:
		++RARP;
		break;

		default: 
		break;
	}
	switch (ip->protocol)
		case 1:
		++ICMP;
		break;

		case 2:
		++IGMP;
		break;

		case 6:
		++TCP;
		break;

		case 17:
		++UDP;
		break;

		default: 
		break;
	}
}

int main(int argc, char *argv[]){
	unsigned char buffer[ETH_FRAME_LEN];
	int sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	struct ifreq ethreq;
	struct sockaddr saddr;
	int saddr_size, rd;

	strncpy(ethreq.ifr_name, "enp0s3", IFNAMSIZ);
	if(ioctl(sock, SIOCGIFFLAGS, &ethreq)==-1){
		perror("ioctl failed!");
		exit(1);
	}
	ethreq.ifr_flags |= IFF_PROMISC;
	if(ioctl(sock, SIOCGIFFLAGS, &ethreq)==-1){
		perror("ioctl failed!");
		exit(1);
	}

	printf("----------Statistics----------\n");
	for(int i=0;i<100;i++){
		saddr_size = sizeof(saddr);
		rd = recvfrom(sock, buffer, sizeof(buffer), 0, &saddr, (socklen_t*)&saddr_size);
		if(rd < 0){
			printf("Recvfrom error , failed to get packets\n");
			return 0;
		}
		count(buffer);
	}

	printf("IP     : %d\n",IP);
	printf("ARP    : %d\n",ARP);
	printf("RARP   : %d\n",RARP);
	printf("TCP    : %d\n",TCP);
	printf("UDP    : %d\n",UDP);
	printf("ICMP   : %d\n",ICMP);
	printf("IGMP   : %d\n",IGMP);
	close(sock);
	printf("-----------finish-----------\n");


	ethreq.ifr_flags &= ~IFF_PROMISC;
	ioctl(sock, SIOCGIFFLAGS, &ethreq);

	return 0;
}

