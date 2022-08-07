#!/usr/bin/env python3
from scapy.all import *


def print_pkt(pkt):
    pkt.show()


inter = ['enp0s3', 'lo']
pkt = sniff(iface=inter, filter='tcp port 23 and src host 10.0.2.5', prn=print_pkt)
