#!/bin/bash

# generic script to kill an application on the old CAVE at the RWTH Aachen University
# requires APLLICATION_NAME_RELEASE and APLLICATION_NAME_DEBUG

########################################
# Generic, application-independet file #
# Don't edit!                          #
# (unless you know what you're doing)  #
########################################

killall $APLLICATION_NAME_RELEASE > /dev/null 2>&1 &
killall $APLLICATION_NAME_DEBUG > /dev/null 2>&1 &
/opt/VR-Software/bin/killcaveclients.sh $APLLICATION_NAME_RELEASE > /dev/null 2>&1 &
/opt/VR-Software/bin/killcaveclients.sh $APLLICATION_NAME_DEBUG > /dev/null 2>&1 &
