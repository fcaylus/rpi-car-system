#!/bin/sh
# This script will launch the app with the correct library path
appname=`basename $0 | sed s,\.sh$,,`
 
dirname=`dirname $0`
tmp="${dirname#?}"
 
if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi
LD_LIBRARY_PATH=$dirname
export LD_LIBRARY_PATH

# Check reboot code
REBOOT_CODE=8956
result=REBOOT_CODE
while [ $result -eq $REBOOT_CODE ]; do
	$dirname/$appname "$@"
	result=$?
done


