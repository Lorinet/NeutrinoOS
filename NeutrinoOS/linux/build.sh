#!/bin/bash

if [ "$EUID" -ne 0 ]
then
    echo "Please run build script as root."
    exit
fi

export BUILDDIR=$(pwd)
export LFS=$BUILDDIR/rootfs
export ARCH=x86_64
export SOURCEDIR=$LFS/sources
export TOOLSDIR=$LFS/tools
export LC_ALL=POSIX
export LFS_TGT=$ARCH-neutrino-linux-gnu
export CONFIG_SITE=$LFS/usr/share/config.site
export MAKEFLAGS='-j16'
export PATH=$PATH:$TOOLSDIR/bin

function createDiskImage {
    echo "Creating disk image..."
    dd if=/dev/zero of=NeutrinoOS.img bs=1M count=64 > /dev/null
    mkfs.ext4 -F NeutrinoOS.img > /dev/null
}

function cleanup {
    echo "Cleaning up..."
    rm -rf $LFS
}

function buildBinutils {
    echo "Extracting binutils source..."
    tar -xf $SOURCEDIR/binutils-2.36.1.tar.xz -C $SOURCEDIR
    cd $SOURCEDIR/binutils-2.36.1
    mkdir -v build
    cd build
    ../configure --prefix=$LFS/tools --with-sysroot=$LFS --target=$LFS_TGT --disable-nls --disable-werror
    make
    make install
}

function buildGCCPass1 {
    echo "Extracting GCC source..."
    tar -xf $SOURCEDIR/gcc-10.2.0.tar.xz -C $SOURCEDIR
    tar -xf $SOURCEDIR/mpfr-4.1.0.tar.xz -C $SOURCEDIR/gcc-10.2.0
    tar -xf $SOURCEDIR/gmp-6.2.1.tar.xz -C $SOURCEDIR/gcc-10.2.0
    tar -xf $SOURCEDIR/mpc-1.2.1.tar.gz -C $SOURCEDIR/gcc-10.2.0
    cd $SOURCEDIR/gcc-10.2.0
    mv -v mpfr-4.1.0 mpfr
    mv -v gmp-6.2.1 gmp
    mv -v mpc-1.2.1 mpc
    case $ARCH in
        x86_64)
            sed -e '/m64=/s/lib64/lib/' -i.orig gcc/config/i386/t-linux64
            ;;
    esac
    mkdir -v build
    cd build
    ../configure --target=$LFS_TGT --prefix=$LFS/tools --with-glibc-version=2.11 --with-sysroot=$LFS --with-newlib --without-headers --enable-initfini-array --disable-nls --disable-shared --disable-multilib --disable-decimal-float --disable-threads --disable-libatomic --disable-libgomp --disable-libquadmath --disable-libssp --disable-libvtv --disable-libstdcxx --enable-languages=c,c++
    make
    make install
    cd ..
    cat gcc/limitx.h gcc/glimits.h gcc/limity.h > `dirname $($LFS_TGT-gcc -print-libgcc-file-name)`/install-tools/include/limits.h
}

function buildLinuxHeaders {
    echo "Extracting Linux source..."
    tar -xf $SOURCEDIR/linux-5.10.17.tar.xz -C $SOURCEDIR
    cd $SOURCEDIR/linux-5.10.17
    make mrproper
    make headers
    find usr/include -name '.*' -delete
    rm usr/include/Makefile
    cp -rv usr/include $LFS/usr
}

function buildGlibc {
    echo "Extracting Glibc source..."
    #tar -xf $SOURCEDIR/glibc-2.33.tar.xz -C $SOURCEDIR
    cd $SOURCEDIR/glibc-2.33
    case $ARCH in
        i?86)
            ln -sfv ld-linux.so.2 $LFS/lib/ld-lsb.so.3
        ;;
        x86_64)
            ln -sfv $SOURCEDIR/lib/ld-linux-x86-64.so.2 $LFS/lib64
            ln -sfv $SOURCEDIR/lib/ld-linux-x86-64.so.2 $LFS/lib64/ld-lsb-x86-64.so.3
        ;;
    esac
    patch -Np1 -i $SOURCEDIR/glibc-2.33-fhs-1.patch
    mkdir -v build
    cd build
    #../configure --prefix=/usr --host=$LFS_TGT --build=$($SOURCEDIR/glibc-2.33/scripts/config.guess) --enable-kernel=3.2 --with-headers=$LFS/usr/include libc_cv_slibdir=/lib
    make
    #make DESTDIR=$LFS install
    #$LFS/tools/libexec/gcc/$LFS_TGT/10.2.0/install-tools/mkheaders
}

function buildLibstdcpp {
    cd $SOURCEDIR/gcc-10.2.0/libstdc++-v3
    mkdir -v build
    cd build
    ../configure --host=$LFS_TGT --build=$($SOURCEDIR/gcc-10.2.0/config.guess) --prefix=/usr --disable-multilib --disable-nls --disable-libstdcxx-pch --with-gxx-include-dir=/tools/$LFS_TGT/include/c++/10.2.0
    make
    make DESTDIR=$LFS install
}   

function buildImage {
    mkdir $LFS
    mkdir $SOURCEDIR
    mkdir -pv $LFS/{bin,etc,lib,sbin,usr,var}
    case $ARCH in
      x86_64) mkdir -pv $LFS/lib64 ;;
    esac
    mkdir -pv $TOOLSDIR
    set +h
    umask 022
    chmod -v a+wt $SOURCEDIR
    wget --input-file=$BUILDDIR/wget-list --continue --directory-prefix=$SOURCEDIR
}

if [ $# -eq 1 ]
then
    $1
fi
