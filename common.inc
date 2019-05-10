### Log Function
## Usage: logger "##### This is a title"
logger () {
dater=$(date +%R" "%D)
echo "${dater} - ${1}" >> ${logs}
}

### Wait for resolution with timeout
## usage: check_timeout "http://myweb.com" "status_variable" 10
check_timeout () {
## Vars
myurl=${1}
myvar=${2}
mytimeout=${3:-60}
mycounter=0

while [ ${mycounter} -lt ${mytimeout} ]; do

        # Resolution Test
        wget -nv --timeout=2 -O - ${myurl}  > /dev/null 2>&1; myerrorlevel=${?}

        if [ "${myerrorlevel}" = "4" ]; then
                eval ${myvar}=1 2>/dev/null
                let mycounter++
                sleep 1s
                logger "> Unable to resolve ${myurl}, trying again. ${mycounter}/${mytimeout}"
        else
                eval ${myvar}=0 2>/dev/null
                break
        fi

done
}

### Wait for service with timeout
## usage: check_service "glusterd" 10
check_service () {
## Vars
mysvc=${1}
mytimeout=${2:-60}
mycounter=0

while [ ${mycounter} -lt ${mytimeout} ]; do

        # Resolution Test
        myoutput=$(systemctl status ${mysvc} 2> /dev/null | grep "active (running)" -c)
		
        if [ "${myoutput}" = "0" ]; then
                myvar=0
                let mycounter++
                sleep 1s
                logger "> Service ${mysvc} not started, trying again. ${mycounter}/${mytimeout}"
        else
                myvar=1
                break
        fi

done

echo ${myvar}
}

### Rancher Metadata Function
## Usage: rancher_meta TypeMeta
rancher_meta () {
TypeMeta=${1}
case $TypeMeta in
     containers)      
          curl "${Base_URL}/service/${TypeMeta}" 2>/dev/null| awk -F"=" '{print $2}' | sort -u
          ;;
     stack_name)      
          curl "${Base_URL}/service/${TypeMeta}" 2>/dev/null
          ;;
     service_index)      
          curl "${Base_URL}/container/${TypeMeta}" 2>/dev/null
          ;;
     name)      
          curl "${Base_URL}/container/${TypeMeta}" 2>/dev/null
          ;;
esac
}

