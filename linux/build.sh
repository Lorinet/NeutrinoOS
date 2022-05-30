#!/bin/bash

echo "Linfinity Linux Build Tool"
echo "<========================>"

shopt -s extglob
export ARCH=i386
export BUILDDIR=$(pwd)
export BUILDROOT=buildroot-2022.02.1

function downloadTools {
    echo "Downloading build tools..."
    sudo apt -y update
    sudo apt -y install sed make binutils build-essential gcc g++ patch perl cpio unzip rsync bc wget git python3
    echo "Downloading Buildroot..."
    wget https://buildroot.org/downloads/buildroot-2022.02.1.tar.gz
    tar -xzf buildroot-2022.02.1.tar.gz
    rm buildroot-2022.02.1.tar.gz
}

function build {
    echo "Configuring Buildroot..."
    cd $BUILDDIR
    echo "Building system..."
    cd $BUILDROOT
    make BR2_EXTERNAL=$BUILDDIR "neutrino_${ARCH}_defconfig"
    make -j16
}

function clean {
    echo "Cleaning up..."
    cd $BUILDROOT
    make clean
}

function rebuild {
    clean
    build
}

function run {
    qemu-system-$ARCH -curses -hda linfinity.linux.img -netdev user,id=mynet0 -device e1000,netdev=mynet0
}

$1