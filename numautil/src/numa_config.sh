#!/bin/bash

VMLIST=(WindowsVM1:0 WindowsVM2:1 ThinVM1:0 ThinVM2:0 ThinVM3:1 ThinVM4:1)

CTON=(`./numautil -c`)

for i in ${CTON[@]}; do
     echo C-N: $i
done

for i in ${VMLIST[@]}; do
     echo Item: $i
done
