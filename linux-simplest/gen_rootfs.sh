#!/usr/bin/env bash

print_usage()
{
	echo "$0 <base tarball>"
	exit
}

if [ $# -lt 1 ]
then
	print_usage
fi

seed=$1

TMP_DIR="temp-rootfs"
TARGET_DIR="target-rootfs"
ROOTFS_NAME="linuxroot.img"

mkdir $TMP_DIR
tar xvf $seed -C $TMP_DIR
dd if=/dev/zero of=$ROOTFS_NAME bs=1M count=512
sudo mkfs.ext4 $ROOTFS_NAME
mkdir $TARGET_DIR
sudo mount $ROOTFS_NAME $TARGET_DIR/
sudo cp -rfp $TMP_DIR/*  $TARGET_DIR/
sudo umount $TARGET_DIR
e2fsck -p -f $ROOTFS_NAME
resize2fs -M $ROOTFS_NAME
qemu-img convert -O qcow2 linuxroot.img linuxroot.qcow2
