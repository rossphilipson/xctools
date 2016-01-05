#!/bin/bash

# The following list specifies what NUMA node to set the VCPU affinity to for
# each of the listed VMs. This is just a sample configuration. The values are
# of the form VMName:NUMANode. Any VMs not listed will not have any node
# affinity set. The primary reason is to allow a guest to only use memory local
# to the CPUs in the node that the VM has affinity to. The second way this is
# useful is for device pass-through where PCI(e) devices have proximity to one
# node or another. E.g. most systems will have thier PCH have proximity to
# NUMA node 0 since by default if there is only one processor package it will
# be in node 0. So it makes sense to assign the Network VM to that since it
# has the integrated NICs assigned to it (those devices being on the PCI(e)
# bus on the PCH.
VMLIST=(Windows7VM1:1 Windows7VM2:1
Windows7VM3:1 Windows7VM4:1 Network:0
WinTPC1:0 WinTPC2:0 WinTPC3:0 WinTPC4:0 WinTPC5:0
WinTPC6:1 WinTPC7:1 WinTPC8:1 WinTPC9:1 WinTPC10:1)

# Dom0 is a special case because it needs its VCPU affinity set at boot time.
# This is done using e.g. "dom0_max_vcpus=4 dom0_vcpus_pin" on the grub command
# line. This says give dom0 4 VCPUs and pin them to the first 4 CPUs. Though
# this is a bit limiting it is mostly fine since dom0 is the domain where all
# the devices on the PCH live that are not passed through. So in this example,
# CPUs 0 - 3 are in NUMA node 0 where the PCH is connected. Setting this value
# to zero will prevent the utility from changing dom0's default configuration.
DOM0=4

# Use utility to get the CPU:NODE mapping for the system
CTON=(`./numautil -c`)

function reset_extra_xenvm {
    echo "Resetting $1"
}

function set_extra_xenvm {
    echo "Setting VM $1 with $3 to $2"
}

# Process the VM list
for i in ${VMLIST[@]}; do
     vm=$(echo $i | cut -d\: -f1)
     node=$(echo $i | cut -d\: -f2)
     vcpus=`xec-vm -n $vm get vcpus`
     echo "Setting node affinity for $vm with $vcpus vcpus to $node"
     # Reset the extra-xenvm node, remove all cpus-affinity settings
     reset_extra_xenvm $vm
     # Set the new CPU affinity configuration
     set_extra_xenvm $vm $node $vcpus
done

# Handle dom0

#for i in ${CTON[@]}; do
#     c=$(echo $i | cut -d\: -f1)
#     n=$(echo $i | cut -d\: -f2)
#     echo "C: $c N: $n"
#done
