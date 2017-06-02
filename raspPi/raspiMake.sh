#! /bin/sh
ROBINhost=raspi3

#scp -rp ./src/* pi@$ROBINhost:src/ROBINsect/servoTest/src/
ssh pi@$ROBINhost 'cd ~/src/test; rm -r src/'
ssh pi@$ROBINhost 'cd ~/src/test; rm -r build/'
#ssh pi@$ROBINhost 'cd ~/src/test; rm -r config/'

scp ./*  pi@$ROBINhost:src/test/
scp -r ./src  pi@$ROBINhost:src/test/src/
#scp -r ./config  pi@$ROBINhost:src/test/config/

ssh pi@$ROBINhost 'cd ~/src/test; ./make_script.sh'
