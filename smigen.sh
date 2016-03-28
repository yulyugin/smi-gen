#!/bin/sh

SMIGEN_DRIVER_KO=build-smigen-`uname -r`/smigen.ko
SMIGEN_EXE=smigen.exe

[ -e $SMIGEN_DRIVER_KO ] || { echo "$SMIGEN_DRIVER_KO does not exist" ; exit 1 ; }
[ -e $SMIGEN_EXE ] || { echo "$SMIGEN_EXE does not exist" ; exit 1 ; }

SUDO=""
if [ ! -z `id -u` ] ; then
   SUDO="sudo"
fi

/sbin/lsmod |grep smigen &> /dev/null && $SUDO /sbin/rmmod smigen
$SUDO /sbin/insmod $SMIGEN_DRIVER_KO

RET=$?
if [ $RET -ne 0 ] ; then
    echo "Failed to load `basename $SMIGEN_DRIVER_KO`"
    exit $RET
fi

$SUDO `readlink -f $SMIGEN_EXE`

