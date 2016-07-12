#!/bin/sh
# This script will launch the app with the correct library path

appname="launcher"
dirname="/opt/rpi-car-system"
export LD_LIBRARY_PATH="/usr/lib/qt/plugins:$dirname"

export QT_QPA_EGLFS_DEPTH=16
export QT_QPA_EGLFS_WIDTH=800
export QT_QPA_EGLFS_HEIGHT=480
export QT_QPA_EGLFS_PHYSICAL_WIDTH=154
export QT_QPA_EGLFS_PHYSICAL_HEIGHT=86
export DISPLAY=:0

# Sames as specified in common.h
REBOOT_CODE=10
UPDATE_CODE=20
SHUTDOWN_CODE=30

count=0

launch () {
	"$dirname/$appname" "$@"
	result=$?

	if [ $result -eq $REBOOT_CODE ]; then
		# check if argument file exist
		if [ -e "$dirname/arguments.txt" ]; then
			args=$(cat "$dirname/arguments.txt")
			launch $args
		else
			launch "$@"
		fi
	elif [ $result -eq $UPDATE_CODE ];then
		# Get content of the update package path
		if [ -e "$dirname/update-package-path" ]; then
			path=$(cat "$dirname/update-package-path")
			"$dirname/updater" --apply-update="$path"
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
	elif [ $result -eq $SHUTDOWN_CODE ]; then
		echo "Shutting down system ..."
		poweroff
	else
		# Unknown return code, try to re-run the launcher
		echo "Unknown error"
		if [ $count -lt 5 ];then
			count=$((count+1))
			launch "$@"
		fi
	fi 
}

launch "$@"


