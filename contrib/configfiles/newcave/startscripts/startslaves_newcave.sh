#!/bin/bash


if [ "$1" = "" ];
then
	echo 'parameter missing - specify a cluster configuration ( FrontWall HoloSpace CaveSpace Angle )'
	exit
fi

if [ "$2" = "" ];
then
	PRIORITY=""
else
	echo using priority $2
	PRIORITY=$2
fi

INPUTCONFIG=${1,,}
CHOSENCONFIG=""

CONFIGS=( FrontWall HoloSpace CaveSpace Angle  )

FrontWall=( 13 14 15 16 )
HoloSpace=( 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 )
CaveSpace=( 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 )
Angle=( 9 10 11 12 13 14 15 16 )

SLAVENAMES=( FloorFront_TL_RE FloorFront_TL_LE FloorFront_BL_RE FloorFront_BL_LE FloorFront_TR_RE FloorFront_TR_LE FloorFront_BR_RE FloorFront_BR_LE FloorBack_TL_RE FloorBack_TL_LE FloorBack_BL_RE FloorBack_BL_LE FloorBack_TR_RE FloorBack_TR_LE FloorBack_BR_RE FloorBack_BR_LE Left_TL_RE Left_TL_LE Left_TR_RE Left_TR_LE Left_BL_RE Left_BL_LE Left_BR_RE Left_BR_LE Front_TL_RE Front_TL_LE Front_TR_RE Front_TR_LE Front_BL_RE Front_BL_LE Front_BR_RE Front_BR_LE Right_TL_RE Right_TL_LE Right_TR_RE Right_TR_LE Right_BL_RE Right_BL_LE Right_BR_RE Right_BR_LE Rear_TL_RE Rear_TL_LE Rear_TR_RE Rear_TR_LE Rear_BL_RE Rear_BL_LE Rear_BR_RE Rear_BR_LE  )

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
	if [ $ID -lt 10 ];
	then
		SLAVEHOSTNAME=linuxgpus0$ID
	else
		SLAVEHOSTNAME=linuxgpus$ID
	fi

	let SLAVENAMEINDEX=$ID-1
	let SLAVENAMEINDEX=$SLAVENAMEINDEX*2
	SLAVENAME_FIRST=${SLAVENAMES[$SLAVENAMEINDEX]]}
	let SLAVENAMEINDEX=$SLAVENAMEINDEX+1
	SLAVENAME_SECOND=${SLAVENAMES[$SLAVENAMEINDEX]]}

	echo start on $SLAVEHOSTNAME as ${SLAVENAME_FIRST}
	ssh $SLAVEHOSTNAME "cd ${DIR}; export XAUTHORITY=/var/run/Xauthority-vr;export DISPLAY=:0.0; ./slave_newcave.sh ${SLAVENAME_FIRST} $@" &

	echo start on $SLAVEHOSTNAME as ${SLAVENAME_SECOND}
	ssh $SLAVEHOSTNAME "cd ${DIR}; export XAUTHORITY=/var/run/Xauthority-vr;export DISPLAY=:0.1; ./slave_newcave.sh ${SLAVENAME_SECOND} $@" &
	
done
