#!/bin/bash

#echo "console=com1 dom0_max_vcpus=4 dom0_mem=min:320M,max:320M,320M" | sed 's/dom0_max_vcpus=[[:digit:]]\+ \?//g'

DOM0=4

function test_grubber() {
    [ ! -f ./x ] && echo "shit" >> ./y

    if [ $DOM0 -gt 0 ]; then
        cat ./grub.cfg | sed 's/dom0_max_vcpus=[[:digit:]]\+ \?//g' | sed 's/dom0_vcpus_pin \?//g' > ./tmp.cfg
        cat ./tmp.cfg | sed "s/XEN_COMMON_CMD=\"/XEN_COMMON_CMD=\"dom0_max_vcpus=$DOM0 dom0_vcpus_pin /" > grub.cfg
    fi
}

function test_reset {
    s="serial=tcp:192.168.2.11:7204,nodelay;cpus-affinity=0:0,1,2,3;cpus-affinity=1:0,1,2,3;usb=ehci"
    value=""

    IFS=';' read -r -a subarr <<< "$s"

    for substr in "${subarr[@]}"
    do
        echo "$substr"
        [[ "$substr" =~ "affinity" ]] && continue

        echo "Go"

        if [ -z "$value" ]; then
            value="$substr"
        else
            value+=";$substr"
        fi
    done

    echo "IN:  $s"
    echo "OUT: $value"
}

test_reset
