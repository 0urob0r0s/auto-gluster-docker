############ Default Configs

WorkVolume=${VolumeName:-gfs}
logs=/var/log/glusterfs/gluster_docker.log

Base_URL="http://rancher-metadata/2015-12-19/self"
svctimeout=${ServiceTimeOut:-15}
wtimeout=${WaitTimeOut:-120}
mountpath=/opt/mount
brickpath=/opt/brick

# Exports Locales
export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8
