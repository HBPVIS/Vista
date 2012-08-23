#!/bin/bash

# generic script to start the slaves of the old CAVE at the RWTH Aachen University

########################################
# Generic, application-independet file #
# Don't edit!                          #
# (unless you know what you're doing)  #
########################################

CONFIGS=( NoSlaves FrontWall HoloSpace CaveSpace SingleSlave )
NoSlaves=( )
FrontWall=( 1 2 )
HoloSpace=( 1 2 3 4 5 6 7 8 9 10 )
CaveSpace=( 1 2 3 4 5 6 7 8 )
SingleSlave=( 1 )

SLAVENAMES=( FrontWallLeft, FrontWallRight, LeftWallLeft, LeftWallRight, RightWallLeft, RightWallRight, BottomLeft, BottomRight, BackLeft, BackRight )


if [ "$1" = "" ];
then
	echo 'parameter missing - specify a cluster configuration ( ${CONFIGS[@]} )'
	exit
fi

INPUTCONFIG=${1,,}
CHOSENCONFIG=""

for TESTCONFIG in ${CONFIGS[@]}
do	
	if [ "${TESTCONFIG,,}" == "$INPUTCONFIG" ];
	then
		CHOSENCONFIG=$TESTCONFIG
		break
	fi
done

if [ "$CHOSENCONFIG" == "" ];
then
	echo "Provided Config does not exist"
	echo "Configs are: ${CONFIGS[@]}"
	exit
else
	echo "Starting config $CHOSENCONFIG"
fi

SLAVELIST="$CHOSENCONFIG[@]"

DIR=`pwd`

for ID in ${!SLAVELIST}
do
	SLAVEHOSTNAME="10.0.0.$ID"	

	let SLAVENAMEINDEX=$ID-1
	SLAVENAME=${SLAVENAMES[$SLAVENAMEINDEX]]}

	echo start on $SLAVEHOSTNAME as ${SLAVENAME_FIRST}
	ssh $SLAVEHOSTNAME "cd ${DIR}; ./slave_oldcave.sh ${SLAVENAME} $@" &
done

