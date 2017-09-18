#!/bin/sh
sudo cp robinsect.service /etc/systemd/system/
sudo chmod u+rw /etc/systemd/system/robinsect.service

sudo systemctl enable robinsect

# Start the service
#sudo systemctl start robinsect

# Stop the service
#sudo systemctl stop robinsect