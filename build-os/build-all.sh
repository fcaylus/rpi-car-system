#!/bin/bash
set -e

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd "$SCRIPT_DIR"
mkdir -pv build
cd build

BUILDROOT_VER="2016.08-rc1"

# Check if a clean is requested
if [ "$1" = "--remove-rpi-car-system-pkg" ]; then
	set +e

	rm -f system-build.done
	rm -f rpi-car-system-sources.tar.gz
	rm -f buildroot-${BUILDROOT_VER}/dl/rpi-car-system-sources.tar.gz
	rm -rf buildroot-${BUILDROOT_VER}/output/build/rpi-car-system
	rm -rf buildroot-${BUILDROOT_VER}/package/rpi-car-system

	cd buildroot-${BUILDROOT_VER}
	make skeleton-rebuild

	exit
fi

#
# Use Buildroot to compile the system
#

if [ ! -f download.done ]; then
	wget -nc http://buildroot.uclibc.org/downloads/buildroot-${BUILDROOT_VER}.tar.gz
	> "$SCRIPT_DIR/build/download.done"
fi

if [ ! -d buildroot-${BUILDROOT_VER} ]; then
    tar xzf buildroot-${BUILDROOT_VER}.tar.gz
fi

cd buildroot-${BUILDROOT_VER}
    
# Copy config files
cp "$SCRIPT_DIR/buildroot-patches/buildroot.config" .config
cp "$SCRIPT_DIR/buildroot-patches/busybox.config" busybox.config
cp "$SCRIPT_DIR/buildroot-patches/kernel.config" kernel.config
cp "$SCRIPT_DIR/buildroot-patches/kernel.patch" kernel.patch
cp -r "$SCRIPT_DIR/buildroot-patches/libvlc" package/
cp -r "$SCRIPT_DIR/buildroot-patches/vlc-qt" package/
cp -r "$SCRIPT_DIR/buildroot-patches/rpi-car-system" package/
cp -r "$SCRIPT_DIR/buildroot-patches/libmediainfo" package/
cp -r "$SCRIPT_DIR/buildroot-patches/libzen" package/

set +e
patch -f -p 1 < "$SCRIPT_DIR/buildroot-patches/Config.in.patch"
set -e

# Create rpi-car-system sources tarball
OLD_PWD=$PWD
cd "$SCRIPT_DIR/.."
tar --exclude="build" --exclude="*~" --exclude="build-os" --exclude=".git" --exclude="Makefile" --exclude="launcher/Makefile" -zcf "$OLD_PWD/../rpi-car-system-sources.tar.gz" .
cd "$OLD_PWD"

make
# Manage licenses files
make legal-info
set +e
cp output/legal-info/manifest.csv output/target/opt/rpi-car-system/licenses
# Remove "rpi-car-system" line since it contains hard coded path
sed -i -n '/rpi-car-system/!p' output/target/opt/rpi-car-system/licenses/manifest.csv
cp -r output/legal-info/licenses/* output/target/opt/rpi-car-system/licenses/files
set -e

SYSTEM_ROOT="${SCRIPT_DIR}/build/buildroot-${BUILDROOT_VER}/output/target"

#
# Copy the official bootloader
#

cd "${SYSTEM_ROOT}/../images/rpi-firmware"

# Copy all files to /boot dir
cp -r bootcode.bin start.elf fixup.dat overlays/ ../../target/boot/

# Rename the kernel
cd "${SYSTEM_ROOT}/boot"
set +e
cp zImage kernel.img
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
install -m 754 etc/rc.d/init.d/udev      "${SYSTEM_ROOT}/etc/rc.d/init.d/"
ln -sf ../init.d/udev "${SYSTEM_ROOT}/etc/rc.d/start/S10udev"
ln -sf ../init.d/udev "${SYSTEM_ROOT}/etc/rc.d/stop/K10udev"

rm "${SYSTEM_ROOT}/etc/inittab"
rm "${SYSTEM_ROOT}/etc/fstab"

install -m 754 etc/inittab "${SYSTEM_ROOT}/etc/"
install -m 644 etc/fstab "${SYSTEM_ROOT}/etc/"
install -m 754 etc/autologin.sh "${SYSTEM_ROOT}/etc/"

#
# Boot files
install -m 644 boot/config.txt "${SYSTEM_ROOT}/boot/"
install -m 644 boot/cmdline.txt "${SYSTEM_ROOT}/boot/"

#
# profile files
rm "${SYSTEM_ROOT}/etc/profile"
install -m 754 etc/profile "${SYSTEM_ROOT}/etc/"

rm "${SYSTEM_ROOT}/etc/group"
install -m 644 etc/group "${SYSTEM_ROOT}/etc/"
install -m 600 etc/shadow "${SYSTEM_ROOT}/etc/"
install -m 644 etc/passwd "${SYSTEM_ROOT}/etc/"

set +e

# Remove useless stuff
rm "${SYSTEM_ROOT}/etc/issue"
rm "${SYSTEM_ROOT}/etc/os-release"
rm "${SYSTEM_ROOT}/etc/services"
rm "${SYSTEM_ROOT}/etc/resolv.conf"
rm "${SYSTEM_ROOT}/etc/protocols"
rm "${SYSTEM_ROOT}/etc/nsswitch.conf"
rm "${SYSTEM_ROOT}/etc/mtab"
#rm "${SYSTEM_ROOT}/etc/hosts"
rm "${SYSTEM_ROOT}/etc/hostname"
rm -r "${SYSTEM_ROOT}/etc/init.d"

rm -r "${SYSTEM_ROOT}/etc/profile.d"
rm -r "${SYSTEM_ROOT}/etc/network"

rm "${SYSTEM_ROOT}/THIS_IS_NOT_YOUR_ROOT_FILESYSTEM"

rm -r "${SYSTEM_ROOT}/usr/share/applications"
rm -r "${SYSTEM_ROOT}/usr/share/gettext"
rm -r "${SYSTEM_ROOT}/usr/share/icons"
rm -r "${SYSTEM_ROOT}/usr/share/kde4"
rm -r "${SYSTEM_ROOT}/usr/share/locale"
rm -r "${SYSTEM_ROOT}/usr/share/vlc"

rm -r "${SYSTEM_ROOT}/usr/qml/QtTest"

rm "${SYSTEM_ROOT}/usr/bin/cvlc"
rm "${SYSTEM_ROOT}/usr/bin/rvlc"
rm "${SYSTEM_ROOT}/usr/bin/libzen-config"

rm -r "${SYSTEM_ROOT}/usr/lib/qt/plugins/qmltooling"
rm "${SYSTEM_ROOT}/usr/lib/"*.py

set -e

echo "All scripts installed !"

#
# Create the final tarball
#

cd "${SYSTEM_ROOT}"

echo "Creating tarball ..."

# When uncompressing the tarball, make sure to pass tar the "-p" switch to ensure permissions are preserved.
tar -Jcf ../../../rpi-car-system.tar.xz ./*

echo "----------------------------------"
echo "Build Finish !!!!"
echo "----------------------------------"



