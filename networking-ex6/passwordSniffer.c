#include <pcap.h>
#include <stdio.h>
#include <arpa/inet.h>


/* Ethernet header */
struct ethheader {
    u_char  ether_dhost[6]; /* destination host address */
    u_char  ether_shost[6]; /* source host address */
    u_short ether_type;     /* protocol type (IP, ARP, RARP, etc) */
};

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

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
        u_short th_sport;               /* source port */
        u_short th_dport;               /* destination port */
        tcp_seq th_seq;                 /* sequence number */
        tcp_seq th_ack;                 /* acknowledgement number */
        u_char  th_offx2;               /* data offset, rsvd */
#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
        u_char  th_flags;
        #define TH_FIN  0x01
        #define TH_SYN  0x02
        #define TH_RST  0x04
        #define TH_PUSH 0x08
        #define TH_ACK  0x10
        #define TH_URG  0x20
        #define TH_ECE  0x40
        #define TH_CWR  0x80
        #define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        u_short th_win;                 /* window */
        u_short th_sum;                 /* checksum */
        u_short th_urp;                 /* urgent pointer */
};

print_payload(const u_char *payload, int len)
{

	const u_char * ch; ch = payload;
     printf ( "Payload: \n\t\t" );
      for ( int i= 0 ; i < len; i++){
           if ( isprint (*ch)){
                if (len == 1 ) {
                     printf ( "\t%c" , *ch);
                      }
                       else { 
                           printf ( "%c" , *ch);
                            } } ch++;
                             } printf ( "\n====\n" );
}

void got_packet(u_char *args, const struct pcap_pkthdr *header,
                const u_char *packet)
{
    const struct sniff_tcp * tcp;
    const char *payload;
    int size_ip;
    int size_tcp; 
    int size_payload; 
    int size_eth;

    struct ethheader *eth = (struct ethheader *)packet;
    size_eth= sizeof(*eth);

    if (ntohs(eth->ether_type) == 0x0800) { // 0x0800 is IP type
        struct ipheader * ip = (struct ipheader *)(packet + sizeof(struct ethheader));
        size_ip = sizeof(ip)* 4 ;
        printf("       From: %s\n", inet_ntoa(ip->iph_sourceip));
        printf("         To: %s\n", inet_ntoa(ip->iph_destip));

    switch(ip->iph_protocol) {
		case IPPROTO_TCP:
			tcp = (struct sniff_tcp*)(packet + size_eth + size_ip);
             size_tcp = TH_OFF(tcp)* 4 ;
              payload = (u_char *)(packet + size_eth + size_ip + size_tcp);
               size_payload = ntohs(ip->iph_len) - (size_ip + size_tcp);

               if (size_payload > 0 ){
                printf ( "Source: %s Port: %d\n" , inet_ntoa(ip->iph_sourceip), ntohs(tcp->th_sport));
                printf ( "Destination: %s Port: %d\n" , inet_ntoa(ip->iph_destip), ntohs(tcp->th_dport));
                print_payload(payload, size_payload); 
                }
			break;
        default :
            printf ( " Protocol: others\n" );
             break;
		
	}

    }
}

int main()
{

    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    char filter_exp[] = "tcp port telnet";
    bpf_u_int32 net;

    // Step 1: Open live pcap session on NIC with name enp0s3
    handle = pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf);

    // Step 2: Compile filter_exp into BPF psuedo-code
    pcap_compile(handle, &fp, filter_exp, 0, net);
    pcap_setfilter(handle, &fp);

    // Step 3: Capture packets
    pcap_loop(handle, -1, got_packet, NULL);

    pcap_close(handle);   //Close the handle
    return 0;
}