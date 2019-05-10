#!/bin/bash
###JAGM@2018

script_path=/root
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

## Load Functions
. /container.env
. ${script_path}/config.inc
. ${script_path}/common.inc
. ${script_path}/gluster.inc


tstart=$(date +'%s' -d $(stat /proc/1/cmdline | grep Modify | sed 's/\./ /'g |awk '{print $3}'))
tend=$(date +%s)
myuptime=$((tend-tstart))

myname=$(rancher_meta name)
myvol=${VolumeName}
current=$(gluster volume status ${myvol} | grep Brick | sed 's/:/ /g' | awk '{print $2}' | sort -n | wc -l)
available=$(rancher_meta containers | wc -l)
myrole=$(gluster --mode=script volume status ${myvol} 2>&1 | grep Brick | grep ${myname} -c)

## Check if a new node has been started and if i'm allowed to add something
if [ "${available}" != "${current}" ] && [ "${myrole}" = "1" ] && [ ${myuptime} -gt 30 ]; then

        for node in $(rancher_meta containers); do
                mylist=$(gluster volume status ${myvol} | grep Brick | sed 's/:/ /g' | awk '{print $2}' | sort -n)
                search_new=$(echo "${mylist// /$'\n'}" | grep ${node} -c)

                if [ "${search_new}" = "0" ]; then
                        logger "#### Finder > New Node - Adding ${node} to the Cluster Volume ${myvol}"
                        mypeer=$(gluster --mode=script peer probe ${node} 2>&1 | grep -ie success -c)

                        if [ "${mypeer}" = "1" ]; then
                                sleep 5s
                                mycnt=$(echo "${mylist// /$'\n'}" | wc -l)
                                let mycnt++
                                adder=$(gluster --mode=script volume add-brick ${myvol} replica ${mycnt} ${node}:${brickpath}/replica_${node} 2>&1)
                                logger "> ${adder}"
                        else
                                logger "> Unable to add node ${node}. I'll try again shortly."
                        fi

                fi

                sleep 1s
        done

fi

