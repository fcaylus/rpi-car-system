#!/bin/sh
# This script will launch the app with the correct library path
appname=$(basename "$0" | sed s,\.sh$,,)
 
dirname=$(dirname "$0")
tmp="${dirname#?}"
 
if [ "${dirname%$tmp}" != "/" ]; then
dirname="$PWD/$dirname"
fi

LD_LIBRARY_PATH=$dirname
export LD_LIBRARY_PATH

# Sames as specified in common.h
REBOOT_CODE=10
UPDATE_CODE=20
SHUTDOWN_CODE=30

launch () {
	"$dirname/$appname" "$@"
	result=$?

	if [ $result -eq $REBOOT_CODE ];then
		launch "$@"
	elif [ $result -eq $UPDATE_CODE ];then
		# Get content of the update package path
		if [ -e "$dirname/update-package-path" ]; then
			"$dirname/updater" --apply-update="$(cat \"$dirname/update-package-path\")"
			result=$?

			if [ $result -eq 0 ]; then
				# Success
				reboot
			else
				# Error
				poweroff
			fi
		else
			# Restart launcher
			launch "$@"
		fi
	elif [ $result -eq $SHUTDOWN_CODE ];then
		poweroff
	else
		# Unknown return code, try to re-run the launcher
		launch "$@"
	fi 
}

launch "$@"


