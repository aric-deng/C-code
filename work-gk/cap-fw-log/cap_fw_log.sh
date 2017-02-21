#!/bin/bash
#dmesg -c


insmod gkdebug.ko
mknod /dev/debug c 248 248

i=0;
while :
do

./logcapture d 4 0 0
./logcapture ./dsplog_enc$i.bin &
sleep 30
killall -9 logcapture
i=`expr $i + 1`

done







