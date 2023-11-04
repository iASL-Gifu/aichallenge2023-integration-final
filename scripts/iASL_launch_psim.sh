#!/bin/bash

source ./vars/vehicle.env

ros2 launch autoware_launch planning_simulator.launch.xml map_path:=../map/AWSIM/ vehicle_model:=$VEHICLE_MODEL sensor_model:=awsim_sensor_kit
