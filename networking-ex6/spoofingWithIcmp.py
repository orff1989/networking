from scapy.all import *
from scapy.layers.inet import IP

a =IP()

a.src='1.2.2.2'
a.dst='10.0.2.15'

b=ICMP()
p=a/b

send(a/b)
ls(p)