#!/bin/bash
set -e

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd "$SCRIPT_DIR"
mkdir -pv build
cd build

#
# Use Build root to compile the system
#

wget -nc http://buildroot.uclibc.org/downloads/buildroot-2015.11-rc3.tar.gz

if [ ! -d buildroot-2015.11-rc3 ]; then
    tar xvzf buildroot-2015.11-rc3.tar.gz
fi

if [ ! -f toolchain.alreadydo ]; then

	cd buildroot-2015.11-rc3
    
    # copy config files
    cp "$SCRIPT_DIR/buildroot-patches/buildroot.config" .config
	cp "$SCRIPT_DIR/buildroot-patches/Config.in" package/Config.in
	cp -r "$SCRIPT_DIR/buildroot-patches/libvlc" package/

    make

    # This file is used to check if the system has been built
    > $SCRIPT_DIR/build/toolchain.alreadydo
else
    echo "System already built !!!"
fi

SYSTEM_ROOT="${SCRIPT_DIR}/build/buildroot-2015.08.1/output/target"

#
# Build the launcher
#

OLD_PATH=$PATH
TOOLCHAIN_DIR="${SCRIPT_DIR}/build/host/usr/bin"
export PATH="$TOOLCHAIN_DIR:$PATH"

cd "$SCRIPT_DIR/build"

mkdir -pv launcher-build
cd launcher-build

SYSROOT="${TOOLCHAIN_DIR}/../arm-buildroot-linux-gnueabihf/sysroot"

cmake -DCMAKE_TOOLCHAIN_FILE="../../scripts/arm-linux.cmake" -DILIXI_INCLUDE_PATH="${SCRIPT_DIR}/scripts/include/ilixi-1.0.0" -DILIXI_LIB_PATH="${SYSTEM_ROOT}/usr/lib" -DDIRECTFB_INCLUDE_PATH="${SYSROOT}/usr/include/directfb" -DSIGCPP_INCLUDE_PATH="${SYSROOT}/usr/include/sigc++-2.0" -DSIGCPP_CONFIG_PATH="${SYSROOT}/usr/lib/sigc++-2.0/include" -DSIGCPP_LIB_PATH="${SYSTEM_ROOT}/usr/lib" -DCMAKE_FIND_ROOT_PATH="${SYSROOT}" -DREADY_FOR_CARSYSTEM=yes -DCMAKE_BUILD_TYPE=Release ../../../launcher/.

make -j6

# Now, copy all files to the system
cd bin/Release
mkdir -pv ${SYSTEM_ROOT}/opt/launcher
cp -r * ${SYSTEM_ROOT}/opt/launcher

export PATH=$OLD_PATH

#
# Copy the official bootloader
#

cd "${SYSTEM_ROOT}/../images/rpi-firmware"

# Copy all files to /boot dir
cp -r bcm2708-rpi-b.dtb bcm2708-rpi-b-plus.dtb bcm2709-rpi-2-b.dtb bootcode.bin start.elf fixup.dat overlays/ ../../target/boot/

# Rename the kernel
cd "${SYSTEM_ROOT}/boot"
set +e
mv zImage kernel.img
set -e

#
# Copy other scripts
#

cd "${SCRIPT_DIR}/scripts"

# 755 for dir
# 754 for executable or scripts
# 644 for config files

#
# Start and stop files
install -d -m 755 "${SYSTEM_ROOT}/etc/rc.d/init.d"
install -d -m 755 "${SYSTEM_ROOT}/etc/rc.d/start"
install -d -m 755 "${SYSTEM_ROOT}/etc/rc.d/stop"
install -m 644 etc/rc.d/init.d/functions "${SYSTEM_ROOT}/etc/rc.d/init.d/"
install -m 754 etc/rc.d/startup          "${SYSTEM_ROOT}/etc/rc.d/"
install -m 754 etc/rc.d/shutdown         "${SYSTEM_ROOT}/etc/rc.d/"
install -m 754 etc/rc.d/init.d/syslog    "${SYSTEM_ROOT}/etc/rc.d/init.d/"
ln -sf ../init.d/syslog "${SYSTEM_ROOT}/etc/rc.d/start/S05syslog"
ln -sf ../init.d/syslog "${SYSTEM_ROOT}/etc/rc.d/stop/K99syslog"

rm ${SYSTEM_ROOT}/etc/inittab
rm ${SYSTEM_ROOT}/etc/fstab

install -m 754 etc/inittab "${SYSTEM_ROOT}/etc/"
install -m 644 etc/fstab "${SYSTEM_ROOT}/etc/"

#
# Boot files
install -m 644 boot/config.txt "${SYSTEM_ROOT}/boot/"
install -m 644 boot/cmdline.txt "${SYSTEM_ROOT}/boot/"

#
# mdev file
install -m 644 etc/mdev.conf "${SYSTEM_ROOT}/etc/"

#
# profile file
rm ${SYSTEM_ROOT}/etc/profile 
install -m 754 etc/profile "${SYSTEM_ROOT}/etc/"
install -m 644 etc/nanorc "${SYSTEM_ROOT}/etc/"

rm ${SYSTEM_ROOT}/etc/group
install -m 644 etc/group "${SYSTEM_ROOT}/etc/"

#
# GUI
install -m 644 etc/directfbrc "${SYSTEM_ROOT}/root"
mv "${SYSTEM_ROOT}/root/directfbrc" "${SYSTEM_ROOT}/root/.directfbrc"

#
# i18n
install -d -m 755 "${SYSTEM_ROOT}/etc/i18n"
install -m 644 fr-latin9.bmap "${SYSTEM_ROOT}/etc/i18n"

set +e

rm ${SYSTEM_ROOT}/etc/issue
rm ${SYSTEM_ROOT}/etc/os-release
rm ${SYSTEM_ROOT}/THIS_IS_NOT_YOUR_ROOT_FILESYSTEM

set -e

echo \"All scripts installed !\"

#
# Create the final tarball
#

cd ${SYSTEM_ROOT}

# When uncompressing the tarball, make sure to pass tar the "-p" switch to ensure permissions are preserved.
tar -jcvf ../../../rpi-car-system.tar.bz2 *

echo "----------------------------------"
echo "Build Finish !!!!"
echo "----------------------------------"



