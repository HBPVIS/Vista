#!/bin/bash

# example for a master_newcave.sh

# no anti-aliasing required on master
export __GL_FSAA_MODE=0
# disable vsync here - state per client will be set by displa_newcave.ini
export __GL_SYNC_TO_VBLANK=0

source ./set_path_for_YOURAPPNAME.sh

if [ "$1" == "" ]; then
	echo "No MASTER config given."
	exit
fi

# setting up environment - these should be set per account, but in case they aren't...
ulimit -c 0 # disable core dumps
ulimit -t unlimited # dont kill jobs after 20mins
export VISTAINIPATH= # unset VISTAINIPATH

# the nice call increases the priority of the application, allowing it to receive signals (e.g. when waiting for
# incoming cluster calls)
/bin/nice_av00 -n 15 YOURAPPNAME -newclustermaster $@ -vistaini vista_newcave.ini -displayini display_newcave.ini -clusterini display_newcave.ini