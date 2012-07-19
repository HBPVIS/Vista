#!/bin/bash

#example for a slave_newcave.sh

# set up anti aliasing
#    __GL_FSAA_MODE     GeForce FX, GeForce 6xxx, GeForce 7xxx, Quadro FX
#    ---------------    ------------------------------------------------------
#    0                  FSAA disabled
#    1                  2x Bilinear Multisampling
#    2                  2x Quincunx Multisampling
#    3                  FSAA disabled
#    4                  4x Bilinear Multisampling
#    5                  4x Gaussian Multisampling
#    6                  2x Bilinear Multisampling by 4x Supersampling
#    7                  4x Bilinear Multisampling by 4x Supersampling
#    8                  4x Bilinear Multisampling by 2x Supersampling
#                       (available on GeForce FX and later GPUs; not
#                       available on Quadro GPUs)
export __GL_FSAA_MODE=8
# disable vsync here - state per client will be set by display_newcave.ini
export __GL_SYNC_TO_VBLANK=0

if [ "$1" == "" ]; then
	echo "No SLAVE config given."
	exit
fi

# setting up environment - these should be set per account, but in case they aren't...
ulimit -c 0 # disable core dumps
ulimit -t unlimited # dont kill jobs after 20mins
export VISTAINIPATH= # unset VISTAINIPATH

# we set the appropriate slave, and pass through all other parameters, too
# the nice call increases the priority of the application, allowing it to receive signals (e.g. when waiting for
# incoming cluster calls)
/bin/nice_av00 -n -15 ./start_newcave.sh -newclusterslave $@

#alternative: redirect all slave output to a file
#if [ ! -d "slavelogs" ]; then
#	mkdir slavelogs
#fi
#/bin/nice_av00 -n -15 ./start_newcave.sh -newclusterslave $@ >slavelogs/log_$1.txt 2>&1
