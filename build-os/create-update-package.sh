#!/bin/bash
set -e

function usage {
    cat <<EOF
Usage: $(basename "$0") [system-package-tarball] [output-package-tarball]
Be careful: Arguments must be absolute file paths !!!
EOF
}

if [ $# -eq 0 ]; then
	usage
	exit 1
fi
if [ $# -eq 1 ]; then
	usage
	exit 2
fi

SYS_PKG="$1"
OUT_PKG="$2"

echo ">>> Creating temp directory ..."
WORK_DIR=$(mktemp -d /tmp/rpi-car-system-update-temp.XXXXXXXXXX)

cd "$WORK_DIR"
mkdir data
cd data

echo ">>> Uncompress system package ..."
tar -xJpvf "${SYS_PKG}"

#
# Only following dirs are used (all others are removed):
# - /bin
# - /etc
# - /lib
# - /opt
# - /sbin
# - /usr/bin
# - /usr/lib
# - /usr/libexec
# - /usr/qml
# - /usr/sbin
# - /usr/share
#

echo ">>> Remove useless files ..."
shopt -s extglob
rm -rf !(bin|etc|lib|opt|sbin|usr)
(
cd usr/ || exit
rm -rf !(bin|lib|libexec|qml|sbin|share)
)

# Copy VERSION file
cp opt/rpi-car-system/VERSION ../

# Generate md5 datas
echo ">>> Generate md5 data ..."
md5deep -lrofbc ./* > ../sums.md5 # Don't follow symlinks

echo ">>> Generate symlinks list ..."
find -type l -fprint ../symlinks.list

cd "${WORK_DIR}"

# Generate final package
echo ">>> Creating the final tarball ..."
tar -Jvcf "${OUT_PKG}" ./*

echo ">>> Remove temp directory ..."
rm -rf "${WORK_DIR}"

echo "----------------------------------"
echo ">>> Update package created !!! <<<"
echo "----------------------------------"

