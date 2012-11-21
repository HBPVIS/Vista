#!/bin/bash

# generic script to start the master on the new CAVE at the RWTH Aachen University

########################################
# Generic, application-independet file #
# Don't edit!                          #
# (unless you know what you're doing)  #
########################################

if [ "$1" == "" ]; then
	echo "master_newcave.sh - No master config given."
	exit -1
fi

export IS_MASTER=true
export CLUSTERCONFIG=$1
shift

# setting up environment - these should be set per account, but in case they aren't...
ulimit -c 0 # disable core dumps
ulimit -t unlimited # don't kill jobs after a certain runtime
export VISTAINIPATH= # unset VISTAINIPATH

# we set the appropriate master, and pass through all other parameters, too
# the nice call increases the priority of the application, allowing it to receive signals faster
# (e.g. when waiting for incoming cluster calls)
/bin/nice_av00 -n -15 ./$STARTSCRIPTS_SUBDIR/start_newcave.sh $@ -newclustermaster $CLUSTERCONFIG
