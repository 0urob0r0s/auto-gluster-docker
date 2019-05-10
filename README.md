# Self Provisioning - Node-aware - Rancher Based Dockerized Gluster Cluster

 **Overview**

This container is based on official Gluster sources from `gluster/gluster-containers`.
It has been modified to automatically deploy a multinode replication cluster
when deployed over a Rancher Environment with at least two nodes.

**Caveats**

This docker image will not work on standard docker environments as it relies on
Rancher's Metadata Service for the autodiscovery feature.

**Features**

- Automatic Gluster replicated volume generation.
- Nodes will reconnect upon restarts or upgrades.
- New nodes will be added automatically to an existing cluster.
- Nodes with integrated healhcheck for self-healing, recovery.

**Usage**

*Example Docker Compose - Cluster Deployment for a Volume named MyVol*

```
version: '2'
services:
  Gluster:
    privileged: true
    cap_add:
    - SYS_ADMIN
    image: 0urob0r0s/rancher-auto-gluster:latest
    environment:
      ServiceTimeOut: '20'
      VolumeName: MyVol
      WaitTimeOut: '120'
    stdin_open: true
    volumes:
    - /dev/:/dev
    - /sys/fs/cgroup:/sys/fs/cgroup:ro
    - /opt/volumes/gluster-rancher/etc:/etc/glusterfs:z
    - /opt/volumes/gluster-rancher/var/lib/glusterd:/var/lib/glusterd:z
    - /opt/volumes/gluster-rancher/var/log/glusterfs:/var/log/glusterfs:z
    - /opt/volumes/gluster-rancher/brick:/opt/brick
    - /opt/volumes/gluster-rancher/mount:/opt/mount:shared
    tty: true
    labels:
      io.rancher.container.hostname_override: container_name
      io.rancher.container.pull_image: always
      io.rancher.scheduler.global: 'true'
 ```
 
*Example Rancher Compose - Provides HealthCheck Configuration*

```
version: '2'
services:
  Gluster:
    retain_ip: true
    start_on_create: true
    health_check:
      healthy_threshold: 2
      response_timeout: 2000
      recreate_on_quorum_strategy_config:
        quorum: 1
      port: 9090
      unhealthy_threshold: 3
      initializing_timeout: 120000
      interval: 2000
      strategy: recreateOnQuorum
      request_line: GET "/" "HTTP/1.0"
      reinitializing_timeout: 60000
```

**Head over the folder `ComposeFiles` on the repo for a more complex example.**
