#!/bin/bash

# generic script to start the slaves of the old CAVE at the RWTH Aachen University

CONFIGS=( NoSlaves FrontWall HoloSpace CaveSpace SingleSlave )
NoSlaves=( )
FrontWall=( 1 2 )
HoloSpace=( 1 2 3 4 5 6 7 8 9 10 )
CaveSpace=( 1 2 3 4 5 6 7 8 )
SingleSlave=( 1 )

#########################################
# Generic, application-independent file #
# Don't edit!                           #
# (unless you know what you're doing)   #
#########################################

SLAVENAMES=( FrontWallLeft, FrontWallRight, LeftWallLeft, LeftWallRight, RightWallLeft, RightWallRight, BottomLeft, BottomRight, BackLeft, BackRight )


# determine configuration
if [ "$1" = "" ];
then
	echo 'parameter missing - specify a cluster configuration ( ${CONFIGS[@]} )'
	exit
fi

INPUTCONFIG=`echo $1  | tr '[A-Z]' '[a-z]'`
CHOSENCONFIG=""

for TESTCONFIG in ${CONFIGS[@]}
do
	TESTCONFIG_LOWERCASE=`echo $TESTCONFIG  | tr '[A-Z]' '[a-z]'`
    if [ "${TESTCONFIG_LOWERCASE}" == "$INPUTCONFIG" ];
    then
        CHOSENCONFIG=$TESTCONFIG
		shift
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

# determine if slave output should be redirected
if $REDIRECT_SLAVE_OUTPUT; then
    
    echo "Redirecting all slaves' output to ./slavelogs/slave_*.log"
    if [ ! -d "slavelogs" ]; then
        mkdir slavelogs
    fi
    
fi

# set prefix for calls (always the same)
DIR=`pwd`
SSH_CALL_PREFIX0="cd ${DIR}; export STARTSCRIPTS_SUBDIR=${STARTSCRIPTS_SUBDIR}"

for ID in ${!SLAVELIST}
do
	SLAVEHOST="10.0.0.$ID"	

	let SLAVENAMEINDEX=$ID-1
    if [ "${!SLAVESKIPVAR}" == "1" ]; then
        echo "${SKIPPED_COLOR}Slave ${ID} skipped by command line param ${RESET_COLOR}"
        continue
    fi

    SLAVENAME_FIRST=${SLAVENODENAME}_RE
    SLAVENAME_SECOND=${SLAVENODENAME}_LE    

    if $REDIRECT_SLAVE_OUTPUT; then

        echo "starting slave ${SLAVENAME} on ${SLAVEHOST}"
        ssh $SLAVEHOST "${SSH_CALL_PREFIX_D0}; ./$STARTSCRIPTS_SUBDIR/slave_newcave.sh ${SLAVENAME} $@ > slavelogs/slave_${SLAVENAME}.log 2>&1" &

    else

        echo "starting slave ${SLAVENAME} on ${SLAVEHOST}"
        ssh $SLAVEHOST "${SSH_CALL_PREFIX_D0}; ./$STARTSCRIPTS_SUBDIR/slave_newcave.sh ${SLAVENAME} $@" &
		
    fi
done

