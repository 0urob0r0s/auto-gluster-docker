#!/bin/bash
while read line;
do
  if [ $(echo $line | wc -c) -lt "3" ]; then
    break
  fi
done

cronchk=$(systemctl status crond | grep Active | awk '{print $2}' | grep ^active -c)
mountchk=$(ls $(mount | grep fuse.glusterfs | awk '{print $3}')>/dev/null 2>&1; echo $?)

# Status code
if [ "${cronchk}" = "0" ] || [ "${mountchk}" != "0" ]; then
	echo HTTP/1.1 404 Not Found
	response="<html><body>inactive</body></html>"
	length=$(echo $response | wc -c)
else
	echo HTTP/1.0 200 OK
	response="<html><body>active</body></html>"
	length=$(echo $response | wc -c)
fi

# Headers and response
echo "Content-Type: text/html; charset=utf-8"
echo "Connection: close"
echo "Content-Length: $length"
echo
echo $response
