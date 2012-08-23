#!/bin/bash

# generic script to kill an application on the new CAVE at the RWTH Aachen University
# requires APLLICATION_NAME_RELEASE and APLLICATION_NAME_DEBUG

########################################
# Generic, application-independet file #
# Don't edit!                          #
# (unless you know what you're doing)  #
########################################

killall $APLLICATION_NAME_RELEASE > /dev/null 2>&1 &
killall InsideD > /dev/null 2>&1 &
/home/vrsw/gpucluster/bin/killcaveclients_nowait.sh $APLLICATION_NAME_RELEASE > /dev/null 2>&1 &
/home/vrsw/gpucluster/bin/killcaveclients_nowait.sh InsideD > /dev/null 2>&1 &
