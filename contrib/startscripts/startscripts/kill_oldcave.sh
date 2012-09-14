#!/bin/bash

# generic script to kill an application on the old CAVE at the RWTH Aachen University
# requires APLLICATION_NAME_RELEASE and APLLICATION_NAME_DEBUG

########################################
# Generic, application-independet file #
# Don't edit!                          #
# (unless you know what you're doing)  #
########################################

killall $APPLICATION_NAME_RELEASE &
killall $APPLICATION_NAME_DEBUG &
/opt/VR-Software/bin/killcaveclients.sh $APPLICATION_NAME_RELEASE &
/opt/VR-Software/bin/killcaveclients.sh $APPLICATION_NAME_DEBUG &
