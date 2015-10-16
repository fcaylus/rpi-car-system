#!/bin/bash
set -e

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

function usage {
    cat <<EOF
Usage: $(basename $0) [params]
  Needed params:
    --dev=SDCARD_DEV      The device name of the SD-Card. Be carefull, this device will be completely wipe !
                            For example: sdd, mmcblk0, ...
  
  Optional params:
    
    --tarball=TAR_NAME    Name of the tarball to flash on the SD-Card, without the extension (default: rpi-car-system) 
    --dir=TAR_DIR         Directory of the tarball (default: $PWD) 
    -h, --help            Show this message
EOF
}

##################
# Default values #
##################

TAR_DIR_NAME="$PWD"
TAR_NAME="rpi-car-system"
SD_CARD_DEVICE=""

#List parameters
args=$(getopt -l "help,dev:,tarball:,dir:" -o "h" -- "$@")

eval set -- "$args"

#Check parameters
while [ $# -ge 1 ] ; do
    case "$1" in
        -h|--help) usage; exit 0;;
        --dev)
            SD_CARD_DEVICE="$2"
            shift;;
        --tarball)
            TAR_NAME="$2";
            shift;;
        --dir)
            TAR_DIR_NAME="$2";
            shift;;
        --) shift; break;;
    esac
    shift
done

# Check needed param
if [ "x$SD_CARD_DEVICE" == "x" ]; then
    echo "You need to specify a device to flash !"
    usage
    exit 1
fi

# Ask for confirmation
echo -n "Are you sure to wipe the device /dev/$SD_CARD_DEVICE and flash it with the rpi-car-system (y/n) ? "
read answer
if echo "$answer" | grep -iq "^y" ;then
    echo "Ok, wipe it !"
else
    echo "Aborted, please re-run the program ..."
    exit 2
fi

#
# Start the program
#

# Get SD-CARD size in MBytes
SD_CARD_SIZE="$(echo $((512*$(cat /sys/class/block/${SD_CARD_DEVICE}/size))) | rev | cut -c 7- | rev)"

# Erase all partitions
sudo dd if=/dev/zero of=/dev/${SD_CARD_DEVICE} bs=512 count=1

# Create a new partition
sudo parted -a optimal -s /dev/${SD_CARD_DEVICE} mklabel msdos
sudo parted -a optimal -s /dev/${SD_CARD_DEVICE} mkpart primary fat32 0 50
sudo parted -a optimal -s /dev/${SD_CARD_DEVICE} mkpart primary ext4 51 ${SD_CARD_SIZE}
sudo parted -a optimal -s /dev/${SD_CARD_DEVICE} set 1 boot on
#sudo parted -a optimal -s /dev/${SD_CARD_DEVICE} set 1 root off
#sudo parted -a optimal -s /dev/${SD_CARD_DEVICE} set 2 root on

# Create the filesystem
sudo mkdosfs -F 32 -n "RPI-BOOT" /dev/${SD_CARD_DEVICE}1
sudo mkfs.ext4 -F -L "RPI-CAR-SYSTEM" /dev/${SD_CARD_DEVICE}2

# Mount the card
sudo mkdir -p /mnt/rpi-card-root /mnt/rpi-card-boot
sudo mount /dev/${SD_CARD_DEVICE}1 /mnt/rpi-card-boot
sudo mount /dev/${SD_CARD_DEVICE}2 /mnt/rpi-card-root

cd /mnt/rpi-card-root

# Flash the tar archive
sudo tar -xjpvf "${TAR_DIR_NAME}/${TAR_NAME}.tar.bz2"

# Set correct permissions
sudo chown -R root:root .
#sudo chgrp -v 13 var/run/utmp var/log/lastlog

sudo mv boot/* /mnt/rpi-card-boot/

echo "Waiting ..."
sleep 2
#sudo umount /dev/${SD_CARD_DEVICE}1 && sudo umount /dev/${SD_CARD_DEVICE}2

echo "----------------------------------"
echo "Flash Finish !!!!"
echo "----------------------------------"

echo "You need to unmount the partions, please run:"
echo "  $ sudo umount /dev/${SD_CARD_DEVICE}1"
echo "  $ sudo umount /dev/${SD_CARD_DEVICE}2"





