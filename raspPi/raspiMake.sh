#! /bin/sh
#scp -rp ./src/* pi@raspi:src/ROBINsect/servoTest/src/

ssh pi@raspi 'cd ~/src/test; rm -r src/'
ssh pi@raspi 'cd ~/src/test; rm -r build/'
#ssh pi@raspi 'cd ~/src/test; rm -r config/'

scp ./*  pi@raspi:src/test/
scp -r ./src  pi@raspi:src/test/src/
#scp -r ./config  pi@raspi:src/test/config/

ssh pi@raspi 'cd ~/src/test; ./make_script.sh'
