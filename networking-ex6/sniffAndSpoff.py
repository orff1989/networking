#!/usr/bin/python
from scapy.all import * 

def sendingPackets(packet):
    if packet[2].type==8:
        id=packet[2].id
        seq=packet[2].seq
        load= packet[3].load
        dst= packet[1].dst
        src= packet[1].src
        print(f"flipping src: {src}, and dst: {dst}") 
        
        theReply = IP(src=dst, dst=src)/ICMP(type= 0 , id=id, seq=seq)/load 
        send(theReply,verbose= 0 )

inter = [ 'enp0s3' , 'lo' ]
pkt = sniff(iface=inter, filter= 'icmp' , prn=sendingPackets)
