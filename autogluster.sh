#!/bin/bash
###JAGM@2018

script_path=/root
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

## Load Functions
. /container.env
. ${script_path}/config.inc
. ${script_path}/common.inc
. ${script_path}/gluster.inc

myname=$(rancher_meta name)
extramount=$(mount | grep fuse.gluster | awk '{print $1}')

### 1 - Check for existing Volume
VolStatus=$(StatusVol ${WorkVolume})

if [ "${VolStatus}" = "" ]; then
	exit 2
fi

if [ "${VolStatus}" = "1" ]; then
	sleep 8s
	umount -l ${myname}:/${WorkVolume} > /dev/null 2>&1
	umount -l ${extramount} > /dev/null 2>&1
	sleep 2s
	MountVol ${WorkVolume}
else
	CreateVol ${WorkVolume}
fi
