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

int main()
{
	int saddr_size , data_size, tcpc=0;
	struct sockaddr saddr;
	struct ifreq ethreq;

	unsigned char *buffer = (unsigned char *) malloc(65536);
	printf("----------Start----------\n");

	int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;

	if(sock_raw < 0)
	{
		perror("Socket Error");
		return 1;
	}
	
	strncpy(ethreq.ifr_name, "enp0s3", IFNAMSIZ);
	if(ioctl(sock_raw, SIOCGIFFLAGS, &ethreq)==-1){
		perror("ioctl failed!");
		exit(1);
	}
	ethreq.ifr_flags |= IFF_PROMISC;
	if(ioctl(sock_raw, SIOCGIFFLAGS, &ethreq)==-1){
		perror("ioctl failed!");
		exit(1);
	}

	while(tcpc<10)
	{
		saddr_size = sizeof saddr;
		data_size = recvfrom(sock_raw , buffer , 65536 , 0 , &saddr , (socklen_t*)&saddr_size);
		if(data_size <0 )
		{
			printf("Recvfrom error , failed to get packets\n");
			return 1;
		}
		struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
		struct ether_header *eth = (struct ether_header *)buffer;
	
		struct sockaddr_in source,dest;
		memset(&source, 0, sizeof(source));
		source.sin_addr.s_addr = iph->saddr;
		memset(&dest, 0, sizeof(dest));
		dest.sin_addr.s_addr = iph->daddr;
		unsigned short iphdrlen;
		iphdrlen = iph->ihl*4;
		struct tcphdr *tcph = (struct tcphdr*)(buffer + iphdrlen  + sizeof(struct ethhdr));

		if(iph->protocol == 6 && strcmp(inet_ntoa(dest.sin_addr), "140.120.15.163") != 0
							  && strcmp(inet_ntoa(source.sin_addr), "140.120.15.163") != 0){

			printf("TCP Packet %d\n", tcpc);
			printf("Source MAC address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->ether_shost[0],
																				 eth->ether_shost[1],
																				 eth->ether_shost[2],
																				 eth->ether_shost[3],
																				 eth->ether_shost[4],
																				 eth->ether_shost[5]);
			printf("Destination MAC address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->ether_dhost[0],
																				 eth->ether_dhost[1],
																				 eth->ether_dhost[2],
																				 eth->ether_dhost[3],
																				 eth->ether_dhost[4],
																				 eth->ether_dhost[5]);
			printf("IP->Protocol            : TCP\n");
			printf("IP->src_ip              : %s\n",inet_ntoa(source.sin_addr));
			printf("IP->dst_ip              : %s\n",inet_ntoa(dest.sin_addr));
			printf("Src_port                : %d\n",ntohs(tcph->source));
			printf("Dst_port                : %d\n",ntohs(tcph->dest));
			printf("-----------------------------------------------------------------\n");
			tcpc++;
		}
	}
	close(sock_raw);
	printf("----------finish----------\n");

	ethreq.ifr_flags &= ~IFF_PROMISC;
	ioctl(sock_raw, SIOCGIFFLAGS, &ethreq);
	return 0;
}
