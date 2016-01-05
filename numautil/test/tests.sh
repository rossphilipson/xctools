#!/bin/bash

#echo "console=com1 dom0_max_vcpus=4 dom0_mem=min:320M,max:320M,320M" | sed 's/dom0_max_vcpus=[[:digit:]]\+ \?//g'

function test_grubber() {
    cat ./grub.cfg | sed 's/dom0_max_vcpus=[[:digit:]]\+ \?//g' | sed 's/ \?dom0_vcpus_pin \?//g' > ./out.cfg
}

function test_reset {
    s="serial=tcp:192.168.2.11:7204,nodelay;cpus-affinity=0:0,1,2,3;cpus-affinity=1:0,1,2,3;usb=ehci"
    value=""

    IFS=';' read -r -a subarr <<< "$s"

    for substr in "${subarr[@]}"
    do
        echo "$substr"
        if [[ "$substr" =~ "affinity" ]] ; then
            echo "It's there!"
            continue
        fi
        echo "Go"

        if [ -z "$value" ]; then
            value="$substr"
        else
            value+=";$substr"
        fi
    done

    echo "Value $value"
}

test_grubber
