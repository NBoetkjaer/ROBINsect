#!/bin/sh
ROBINhost=raspi

RELATIVEDIR="src/test/"
BASEDIR="\$HOME/$RELATIVEDIR"

# Stop robinsect program/service.
ssh pi@$ROBINhost "sudo systemctl stop robinsect"
# Make directory and transfer source files.
ssh pi@$ROBINhost "mkdir -p $BASEDIR"

ssh pi@$ROBINhost "cd $BASEDIR; rm -r src/"
ssh pi@$ROBINhost "cd $BASEDIR; rm -r boot/"
ssh pi@$ROBINhost "cd $BASEDIR; rm -r build/"
#ssh pi@$ROBINhost "cd $BASEDIR; rm -r config/'

scp ./*  pi@$ROBINhost:${RELATIVEDIR}
scp -r ./src pi@$ROBINhost:${RELATIVEDIR}src/
scp -r ./boot pi@$ROBINhost:${RELATIVEDIR}boot/
#scp -r ./config  pi@$ROBINhost:src/test/config/

ssh pi@$ROBINhost "cd $BASEDIR; ./make_script.sh"
# Start robinsect program/service.
ssh pi@$ROBINhost "sudo systemctl start robinsect"