#!/bin/bash

VMLIST=(VM1:0 VM2:0 VM3:1 VM4:1)         

for i in ${VMLIST[@]}; do
     echo Item: $i
done
