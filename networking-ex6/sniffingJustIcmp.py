#!/usr/bin/env python3
from scapy.all import*

def print_pkt(pkt):
    pkt.show()
    

inter= ['enp0s3','lo']
pkt = sniff(iface=inter, filter='icmp', prn=print_pkt)