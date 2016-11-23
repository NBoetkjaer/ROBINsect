#! /bin/sh
#scp -rp ./src/* pi@raspi:src/ROBINsect/servoTest/src/
scp -rp ./*  pi@raspi:src/test/
ssh pi@raspi 'cd ~/src/test; ./make_script.sh'