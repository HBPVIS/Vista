#!/bin/bash

# generic script to kill an application on the new CAVE at the RWTH Aachen University
# requires APLLICATION_NAME_RELEASE and APLLICATION_NAME_DEBUG

########################################
# Generic, application-independet file #
# Don't edit!                          #
# (unless you know what you're doing)  #
########################################

killall $APPLICATION_NAME_RELEASE &
killall $APPLICATION_NAME_DEBUG &
/home/vrsw/gpucluster/bin/killcaveclients_nowait.sh $APPLICATION_NAME_RELEASE  &
/home/vrsw/gpucluster/bin/killcaveclients_nowait.sh $APPLICATION_NAME_DEBUG &
