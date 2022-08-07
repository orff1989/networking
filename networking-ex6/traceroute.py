from scapy.all import *

inRoute = True
j = 1
while inRoute:
    a = IP(dst= '157.240.196.35' , ttl=j)
    p=a/ICMP()
    theResponse = sr1(p,timeout= 8 ,verbose= 0 )

    if theResponse is None:
        print(f" {j} Request timed out.") 
    elif theResponse.type == 0: 
        print(f" {j} - {theResponse.src}") 
        inRoute = False 
    else : 
        print(f" {j} - {theResponse.src}") 
    j = j + 1 
