### StatusVolume Function
## Usage: StatusVol VolumeName
StatusVol () {
myvol=${1}

	logger "#### Volume ${myvol} Status"
	svc_check=$(check_service "glusterd" ${svctimeout})
	
	if [ "${svc_check}" = "1" ]; then
		mystatus=$(gluster volume info ${myvol} 2>&1 | grep "Volume Name: ${myvol}" -c)
		
		if [ "${mystatus}" = "1" ]; then
			logger "> Volume ${myvol} Detected."
		else
			logger "> Volume ${myvol} doesn't exist"
		fi
	else
		logger "> Glusterd Service Failed to Start. Aborting operation..."
		mystatus=""
		exit 2
	fi
	
	echo ${mystatus}
}


### MountVolume Function
## Usage: MountVol VolumeName
MountVol () {
myvol=${1}
myname=$(rancher_meta name)

	logger "#### Mounting volume ${myvol}"
	mkdir ${mountpath}/${myvol} 2> /dev/null
	myoutput=$(mount -t glusterfs ${myname}:/${myvol} ${mountpath}/${myvol} 2>&1)
	mystatus=${?}
	
	if [ "${mystatus}" = "0" ]; then
		logger "> Mount successful at ${mountpath}/${myvol}"
	else
		logger "> Mount error!!"
		logger "> > ${myoutput}"
	fi
}


### WaitVolume Function
## Usage: WaitVol VolumeName
WaitVol () {
myvol=${1}
myname=$(rancher_meta name)
mytimeout=${wtimeout}
mycounter=0

	logger "#### Waiting for node ${myname} to be added to the cluster."
	while [ ${mycounter} -lt ${mytimeout} ]; do

		# Resolution Test
		myoutput=$(gluster --mode=script volume status ${myvol} 2>&1 | grep Brick | grep ${myname} -c)
								
		if [ "${myoutput}" = "0" ]; then
			addout=0
			let mycounter++
			sleep 1s
			logger "> > Node ${myname} not added yet, trying again. ${mycounter}/${mytimeout}"
		else
			addout=1
			mycounter=${mytimeout}
		fi

	done

	if [ "${addout}" = "1" ]; then
		logger "> > Added Successfully"
		gluster --mode=script volume start ${myvol} > /dev/null 2>&1
		extramount=$(mount | grep fuse.gluster | awk '{print $1}')
		umount ${myname}:/${WorkVolume} > /dev/null 2>&1
	    umount ${extramount} > /dev/null 2>&1
		sleep 5s
		MountVol ${myvol}
	else	
		logger "> > Unable to add to existing Cluster."
	fi
}


### CreateVolume Function
## Usage: CreateVol VolumeName
CreateVol () {

myvol=${1}
mysvcnt=$(rancher_meta containers | wc -l)
myself=$(rancher_meta service_index)
myname=$(rancher_meta name)
nodecount=0

	logger "#### Creating volume ${myvol}"
	mkdir /opt/mount/${myvol} 2> /dev/null
	
	### Generate Pool and test Reachability
	for node in $(rancher_meta containers); do 
	
		# Check if this is a new node for an existing cluster
		chknewnode=$(gluster --mode=script peer probe ${node} 2>&1 | grep -ie failed | grep "part of another cluster" -c)
		if [ "${chknewnode}" = "1" ]; then
			logger "> Node ${myname} detected as new. Waiting for cluster to add it."
			WaitVol ${myvol}
			exit 0
		fi
				
		logger "> Probe Node ${node}"
		mytimeout=${svctimeout:-60}
		mycounter=0
		while [ ${mycounter} -lt ${mytimeout} ]; do

				# Resolution Test
				myoutput=$(gluster --mode=script peer probe ${node} 2>&1 | grep -ie success -c)
								
				if [ "${myoutput}" = "0" ]; then
						addout=0
						let mycounter++
						sleep 1s
						logger "> > Peer probe ${node} unsuccessful, trying again. ${mycounter}/${mytimeout}"
				else
						addout=1
						mycounter=${mytimeout}
				fi

		done
					
		if [ "${addout}" = "1" ]; then
			my_node_array[${nodecount}]=${node}
			let nodecount++
			logger "> > Probe Successful"
		else	
			logger "> > Probe Failed"
		fi

	done
	
	## Exit if nodecount < 2
    if [ ${nodecount} -lt 2 ]; then
		logger "#### Unable to create a Replication Cluster with less than 2 nodes"
		exit 2
    fi
	
	### Check if I'm the number one! and create the Cluster
	if [ "${myself}" = "1" ]; then
		logger "#### Primary - Creating the Cluster Volume ${myvol}"
		mybrkcnt=0
		
		for node in $(echo ${my_node_array[@]}); do
			my_bricks[${mybrkcnt}]="${node}:${brickpath}/replica_${node}"
			let mybrkcnt++
		done
		
		# Cluster Generation
		createout=$(gluster --mode=script volume create ${myvol} replica ${nodecount} ${my_bricks[@]} force 2>&1 | grep -ie success -c)
		if [ "${createout}" = "1" ]; then
			logger "> > Cluster creation Successful"
		else	
			logger "> > Cluster creation Failed"
			exit 2
		fi		
	fi
	
	### If I'm not number one, I'm a founder node?
	if [ "${myself}" != "1" ]; then
	
		mytimeout=${svctimeout:-60}
		mycounter=0
		
		while [ ${mycounter} -lt ${mytimeout} ]; do
				# Resolution Test
				myoutput=$(gluster volume info ${myvol} 2>&1 | grep Brick | grep ${myname} -c)
				
				if [ "${myoutput}" = "0" ]; then
						founder=0
						let mycounter++
						sleep 1s
						logger "> > Validating ${myname} role. ${mycounter}/${mytimeout}"
				else
						founder=1
						mycounter=${mytimeout}
				fi
		done
		
		## Founder or existing Node
		if [ "${founder}" = "1" ]; then
			logger "#### Founder Node - Starting the Cluster Volume ${myvol}"
		fi
		
	fi
	
	## Start Node
	gluster --mode=script volume start ${myvol} > /dev/null 2>&1
	
	## Mount it
	extramount=$(mount | grep fuse.gluster | awk '{print $1}')
	umount ${myname}:/${WorkVolume} > /dev/null 2>&1
	umount ${extramount} > /dev/null 2>&1
	sleep 5s
	MountVol ${myvol}
}






