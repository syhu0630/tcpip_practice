#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/ip_icmp.h>
#include<netinet/ip.h>
#include<sys/time.h> 

short checksum(void *buf, int len){
	unsigned short *buffer = buf;
	unsigned int sum=0;
	unsigned short result;
	
	for(sum=0;len>1;len-=2){
		sum += *buffer++;
	}	
	if(len==1){
		sum += *(unsigned char*)buffer;
	} 
	sum = (sum>>16) + (sum&0xFFFF);
	sum += (sum>>16);
	result = ~sum;
	return result;
}

int main(int argc, char *argv[]){
	struct sockaddr_in addr;
	struct timeval start, end;
	struct icmp s_packet;
    char recvbuf[256]; 
	int seq=0, sd, rd;
	int tssend, tsrecv, rtt, tsfinal=0;
	int sock = socket(PF_INET,SOCK_RAW,1);
	if(sock<0){
		printf("Error creating socket\n");
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = PF_INET;
	addr.sin_port = 0;
	addr.sin_addr.s_addr = inet_addr(argv[1]);

	gettimeofday(&start, NULL);
	tssend = (start.tv_sec % (24 * 3600)) * 1000 + start.tv_usec /1000;

	bzero(&s_packet, sizeof(s_packet));
	s_packet.icmp_type=13;
	s_packet.icmp_code=0;
	s_packet.icmp_id=getpid();
	s_packet.icmp_seq=seq;
	s_packet.icmp_otime=htonl(tssend);
	s_packet.icmp_cksum=checksum(&s_packet, sizeof(s_packet));
	printf("icmp_type = %d, icmp_code = %d\n", s_packet.icmp_type, s_packet.icmp_code);
	printf("Original timestamp = %d\n"       , ntohl(s_packet.icmp_otime));
	printf("Receive timestamp = %d\n"        , s_packet.icmp_rtime);
	printf("Transmit timestamp = %d\n"       , s_packet.icmp_ttime);
	printf("Final timestamp = %d\n"          , tsfinal);
	printf("---------------------------------\n");
	sd = sendto(sock, &s_packet, sizeof(struct icmp), 0, (struct sockaddr *)&addr, sizeof(addr));
	if(sd == -1){
		printf("send error");
	}
	
	while(1){
		rd = recvfrom(sock, &recvbuf, sizeof(recvbuf), 0, NULL, NULL);
		if(rd == -1){
			printf("recv error");
		}
		gettimeofday(&end, NULL);
		tsrecv = (end.tv_sec % (24 * 3600)) * 1000 + end.tv_usec /1000;
		struct iphdr *recvip = (struct iphdr *)recvbuf;
		struct icmp *r_packet = (struct icmp *)(recvbuf + (recvip->ihl<<2));
		if(r_packet->icmp_type == 14){
			tsfinal = tsrecv;
			rtt = (tsfinal - ntohl(r_packet->icmp_ttime)) + (ntohl(r_packet->icmp_rtime) - ntohl(r_packet->icmp_otime));
			printf("icmp_type = %d, icmp_code = %d\n", r_packet->icmp_type, r_packet->icmp_code);
			printf("Original timestamp = %u\n"       , ntohl(r_packet->icmp_otime));
			printf("Receive timestamp = %u\n"        , ntohl(r_packet->icmp_rtime));
			printf("Transmit timestamp = %u\n"       , ntohl(r_packet->icmp_ttime));
			printf("Final timestamp = %u\n"          , tsfinal);
			printf("RTT = %u\n"                      , rtt);
			printf("---------------------------------\n");
			break;
		}
	}
	return 0;
}


