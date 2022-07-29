#!/bin/bash
BUILDDIR=$2
BOARD_DIR=$(dirname "$0")
echo "Creating disk image..."
cd $BUILDDIR
dd if=/dev/zero of=$BUILDDIR/linfinity.linux.img bs=1M count=96 > /dev/null
echo "Setting up loopback device..."
lodev=$(sudo losetup -f)
sudo losetup $lodev $BUILDDIR/linfinity.linux.img
cat << EOF | sudo fdisk $lodev
o
n
p
1
2048

a
p
w
q
EOF
echo "Remounting loopback device..."
sudo losetup -d $lodev
lodev=$(losetup -f)
sudo losetup -P $lodev $BUILDDIR/linfinity.linux.img
echo "Formatting partition..."
sudo mkfs -t ext4 ${lodev}p1
mkdir $BUILDDIR/imgdir
echo "Mounting image..."
sudo mount ${lodev}p1 $BUILDDIR/imgdir
echo "Copying files..."
sudo tar -xf $BINARIES_DIR/rootfs.tar -C $BUILDDIR/imgdir
sudo rm -rf $BUILDDIR/imgdir/{media,mnt,opt,root}
echo "Installing kernel..."
sudo cp $BINARIES_DIR/bzImage $BUILDDIR/imgdir/boot/
echo "Installing Grub2 bootloader..."
grub_tgt="${ARCH}-pc"
case $ARCH in
    i686)
        grub_tgt="i386-pc"
        ;;
esac
pushd $BUILD_DIR
btd=$(find . -maxdepth 1 -type d -name '*grub2-*' -print -quit)
sudo $BASE_DIR/host/sbin/grub-bios-setup -b output/build/$btd/build-i386-pc/grub-core/boot.img -c output/images/grub.img -d $CONFIG_DIR $lodev
popd
echo "Unmounting..."
sudo umount -l $BUILDDIR/imgdir
sudo losetup -d $lodev
echo "Cleaning up..."
sudo rm -rf $BUILDDIR/imgdir
echo "Linfinity Linux bootable image linfinity.linux.img created successfully! Have a nice day :)"

