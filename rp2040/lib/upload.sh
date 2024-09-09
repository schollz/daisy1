#!/bin/bash

while true
do
if [ -d "/media/zns/RPI-RP2" ]; then
	sleep 0.5
	echo "uploading!"
	time pv -batep build/*.uf2 > /media/zns/RPI-RP2/_core.uf2
	exit
fi
sleep 0.1
done