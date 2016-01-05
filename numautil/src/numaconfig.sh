#!/bin/bash

VMLIST=(Windows7VM1:0 Windows7VM2:0
Windows7VM3:1 Windows7VM4:1 Domain0:0 Network:0
WinTPC1:0 WinTPC2:0 WinTPC3:0 WinTPC4:0 WinTPC5:0
WinTPC6:1 WinTPC7:1 WinTPC8:1 WinTPC9:1 WinTPC10:1)

#CTON=(`./numautil -c`)
CTON=(`../test/numautil -c`)

for i in ${VMLIST[@]}; do
     vm=$(echo $i | cut -d\: -f1)
     node=$(echo $i | cut -d\: -f2)
     echo "Setting node affinity for $vm to $node"
done

for i in ${CTON[@]}; do
     c=$(echo $i | cut -d\: -f1)
     n=$(echo $i | cut -d\: -f2)
     echo "C: $c N: $n"
done
