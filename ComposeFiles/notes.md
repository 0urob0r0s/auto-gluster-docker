# Notes:
This example deployment provides a distributed, replicated, loadbalanced and highly available SMB Service by integrating SMB + Gluster + LB.
For proper operation a minimum of 3 nodes is required deployed over a Cattle Rancher Environment.

# Post Deployment Steps

Provide write privileges to you Shared folder to be accesible through the SAMBA Service.
On any of the docker hosts run the following:
```
chmod ugoa+w /opt/volumes/gluster-rancher/mount/NewStore
```
