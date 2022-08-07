#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <arpa/inet.h>
/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN 6
#define PACKET_LEN 512
/* IP Header */
struct ipheader {
    unsigned char      iph_ihl:4, //IP header length
    iph_ver:4; //IP version
    unsigned char      iph_tos; //Type of service
    unsigned short int iph_len; //IP Packet length (data + header)
    unsigned short int iph_ident; //Identification
    unsigned short int iph_flag:3, //Fragmentation flags
    iph_offset:13; //Flags offset
    unsigned char      iph_ttl; //Time to Live
    unsigned char      iph_protocol; //Protocol type
    unsigned short int iph_chksum; //IP datagram checksum
    struct  in_addr    iph_sourceip; //Source IP address
    struct  in_addr    iph_destip;   //Destination IP address
};
/* Ethernet header */
struct ethheader {
    u_char  ether_dhost[ETHER_ADDR_LEN]; /* destination host address */
    u_char  ether_shost[ETHER_ADDR_LEN]; /* source host address */
    u_short ether_type;                  /* IP? ARP? RARP? etc */
};
/* ICMP Header  */
struct icmpheader {
    unsigned char icmp_type; // ICMP message type
    unsigned char icmp_code; // Error code
    unsigned short int icmp_chksum; //Checksum for ICMP Header and data
    unsigned short int icmp_id;     //Used for identifying request
    unsigned short int icmp_seq;    //Sequence number
};
/* UDP Header */
struct udpheader
{
    u_int16_t udp_sport;           /* source port */
    u_int16_t udp_dport;           /* destination port */
    u_int16_t udp_ulen;            /* udp length */
    u_int16_t udp_sum;             /* udp checksum */
};
/* TCP Header */
struct tcpheader {
    u_short tcp_sport;               /* source port */
    u_short tcp_dport;               /* destination port */
    u_int   tcp_seq;                 /* sequence number */
    u_int   tcp_ack;                 /* acknowledgement number */
    u_char  tcp_offx2;               /* data offset, rsvd */
#define TH_OFF(th)      (((th)->tcp_offx2 & 0xf0) >> 4)
    u_char  tcp_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short tcp_win;                 /* window */
    u_short tcp_sum;                 /* checksum */
    u_short tcp_urp;                 /* urgent pointer */
};
/* Psuedo TCP header */
struct pseudo_tcp
{
    unsigned saddr, daddr;
    unsigned char mbz;
    unsigned char ptcl;
    unsigned short tcpl;
    struct tcpheader tcp;
    char payload[PACKET_LEN];
};
/*************************************************************
  Given an IP packet, send it out using a raw socket.
**************************************************************/
void send_raw_ip_packet(struct ipheader* ip)
{
    struct sockaddr_in dest_info;
    int enable = 1;

    // Step 1: Create a raw network socket.
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    // Step 2: Set socket option.
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL,
               &enable, sizeof(enable));

    // Step 3: Provide needed information about destination.
    dest_info.sin_family = AF_INET;
    dest_info.sin_addr = ip->iph_destip;

    // Step 4: Send the packet out.
    sendto(sock, ip, ntohs(ip->iph_len), 0,
           (struct sockaddr *)&dest_info, sizeof(dest_info));
    close(sock);
}
/******************************************************************
  Spoof a UDP packet using an arbitrary source IP Address and port
*******************************************************************/
int main() {
    char buffer[1500];

    memset(buffer, 0, 1500);
    struct udpheader *udp = (struct udpheader *) (buffer +
                                                  sizeof(struct ipheader));

    /*********************************************************
       Step 1: Fill in the UDP data field.
     ********************************************************/
    char *data = buffer + sizeof(struct ipheader) +
                 sizeof(struct udpheader);
    const char *msg = "Hello Server!\n";
    int data_len = strlen(msg);
    strncpy(data, msg, data_len);

    /*********************************************************
       Step 2: Fill in the UDP header.
     ********************************************************/
    udp->udp_sport = htons(12345);
    udp->udp_dport = htons(9090);
    udp->udp_ulen = htons(sizeof(struct udpheader) + data_len);
    udp->udp_sum = 0; /* Many OSes ignore this field, so we do not
                         calculate it. */

    /*********************************************************
       Step 3: Fill in the IP header.
     ********************************************************/

    /*Same as in ICMP Echo step 2 . */
    struct ipheader *ip = (struct ipheader *) buffer;
    ip->iph_ver = 4;
    ip->iph_ihl = 5;
    ip->iph_ttl = 20;
    ip->iph_sourceip.s_addr = inet_addr("1.2.3.4");
    ip->iph_destip.s_addr = inet_addr("10.0.2.15");

    ip->iph_protocol = IPPROTO_UDP; // The value is 17.
    ip->iph_len = htons(sizeof(struct ipheader) +
                        sizeof(struct udpheader) + data_len);

    /*********************************************************
       Step 4: Finally, send the spoofed packet
     ********************************************************/
    send_raw_ip_packet(ip);

    return 0;

}
