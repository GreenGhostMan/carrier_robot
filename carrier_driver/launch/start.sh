#!/usr/bin/env bash
xterm -e roslaunch carrier_driver driver.launch &
sleep 5
xterm -e roslaunch carrier_driver twist.launch

#xterm -e roslaunch carrier_driver multi_driver.launch &
#sleep 5
#xterm -e roslaunch carrier_driver multi_twist.launch
