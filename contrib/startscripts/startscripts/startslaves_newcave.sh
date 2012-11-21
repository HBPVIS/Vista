#!/bin/bash


# generic script to start the slaves of the new aixCAVE at RWTH Aachen University

# define the colors for node status messages
DISABLED_COLOR=$(echo -e "\033[0;30;41m") # black on red
SKIPPED_COLOR=$(echo -e "\033[0;30;41m") # black on red
WARN_COLOR=$(echo -e "\033[0;30;43m") # black on yellow/brown
IPCHANGED_COLOR=$(echo -e "\033[1;32m") # green
RESET_COLOR=$(echo -e "\033[0m")

CONFIGS=( NoSlaves LeftWall FrontWall RightWall Door HoloSpace CaveSpace Angle AngleSpace CaveSpaceNoFloor HoloSpaceNoFloor  )
NoSlaves=( )
LeftWall=( 9 10 11 12 )
FrontWall=( 13 14 15 16 )
RightWall=( 17 18 19 20 )
Door=( 21 22 23 24 )
HoloSpace=( 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 )
CaveSpace=( 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 )
Angle=( 9 10 11 12 13 14 15 16 )
AngleSpace=( 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 )
CaveSpaceNoFloor=( 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 )
HoloSpaceNoFloor=( 9 10 11 12 13 14 15 16 17 18 19 20 )

#########################################
# Generic, application-independent file #
# Don't edit!                           #
# (unless you know what you're doing)   #
#########################################

# load slave ip config settings to know each slave's current IP
if [ "$SLAVENODES_CONFIGURATION_FILE" == "" ]; then
	if [ "$newcave_SLAVENODES_CONFIGURATION_FILE" == "" ]; then
		echo "No valid slave configuration file set - resetting to default"
		$SLAVENODES_CONFIGURATION_FILE="/home/vrsw/gpucluster/slavenodes_ipconfig_newcave.sh"
	else
		$SLAVENODES_CONFIGURATION_FILE = $newcave_SLAVENODES_CONFIGURATION_FILE
	fi
fi

if ! [ -f "$SLAVENODES_CONFIGURATION_FILE" ]; then
	echo "${DISABLED_COLOR}Could not load slave configuration file \"$SLAVENODES_CONFIGURATION_FILE\"${RESET_COLOR}"
	exit	
fi
source $SLAVENODES_CONFIGURATION_FILE

# determine configuration
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
SSH_CALL_PREFIX_D0="cd ${DIR}; export XAUTHORITY=/var/run/Xauthority-vr; export DISPLAY=:0.0; export SLAVENODES_CONFIGURATION_FILE=${SLAVENODES_CONFIGURATION_FILE}; export STARTSCRIPTS_SUBDIR=${STARTSCRIPTS_SUBDIR}"
SSH_CALL_PREFIX_D1="cd ${DIR}; export XAUTHORITY=/var/run/Xauthority-vr; export DISPLAY=:0.1; export SLAVENODES_CONFIGURATION_FILE=${SLAVENODES_CONFIGURATION_FILE}; export STARTSCRIPTS_SUBDIR=${STARTSCRIPTS_SUBDIR}"

for ID in ${!SLAVELIST}
do
    
    let SLAVENODEINDEX=$ID-1
    if [ $ID -lt 10 ];
    then
        SLAVENODESECTION=SLAVENODE0${ID}
        SLAVESKIPVAR=SKIP_SLAVE0${ID}
    else
        SLAVENODESECTION=SLAVENODE${ID}
        SLAVESKIPVAR=SKIP_SLAVE${ID}
    fi
    
    eval SLAVENODENAME=\${$SLAVENODESECTION[0]}
    eval SLAVEHOST=\${$SLAVENODESECTION[1]}
    eval SLAVESTATUS=\${$SLAVENODESECTION[3]}
    eval SLAVEMESSAGE=\${$SLAVENODESECTION[4]}
        
    if [ "${SLAVESTATUS}" == "DISABLED" ]; then
        echo "${DISABLED_COLOR}##########################################"
        echo "# Slave ${ID} currently disabled "
        echo "# ${SLAVEMESSAGE}"
        echo "##########################################${RESET_COLOR}"
        continue
    elif [ "${SLAVESTATUS}" == "WARNING" ]; then
        echo "${WARN_COLOR}###########################################"
        echo "# Slave ${ID} WARNING:  ${SLAVEMESSAGE}"
        echo "##########################################${RESET_COLOR}"
    elif [ "${SLAVESTATUS}" == "IP_CHANGED" ]; then
        echo "${IPCHANGED_COLOR}Slave ${ID} has changed IP:   ${SLAVEMESSAGE}${RESET_COLOR}"
    fi
    
    if [ "${!SLAVESKIPVAR}" == "1" ]; then
        echo "${SKIPPED_COLOR}Slave ${ID} skipped by command line param ${RESET_COLOR}"
        continue
    fi

    SLAVENAME_FIRST=${SLAVENODENAME}_RE
    SLAVENAME_SECOND=${SLAVENODENAME}_LE    

    if $REDIRECT_SLAVE_OUTPUT; then

        echo "starting slave ${SLAVENAME_FIRST} on ${SLAVEHOST}"
        ssh $SLAVEHOST "${SSH_CALL_PREFIX_D0}; ./$STARTSCRIPTS_SUBDIR/slave_newcave.sh ${SLAVENAME_FIRST} $@ > slavelogs/slave_${SLAVENAME_FIRST}.log 2>&1" &

        echo "starting slave ${SLAVENAME_SECOND} on ${SLAVEHOST}"
        ssh $SLAVEHOST "${SSH_CALL_PREFIX_D1}; ./$STARTSCRIPTS_SUBDIR/slave_newcave.sh ${SLAVENAME_SECOND} $@ > slavelogs/slave_${SLAVENAME_SECOND}.log 2>&1" &
    
    else

        echo "starting slave ${SLAVENAME_FIRST} on ${SLAVEHOST}"
        ssh $SLAVEHOST "${SSH_CALL_PREFIX_D0}; ./$STARTSCRIPTS_SUBDIR/slave_newcave.sh ${SLAVENAME_FIRST} $@" &

        echo "starting slave ${SLAVENAME_SECOND} on ${SLAVEHOST}"
        ssh $SLAVEHOST "${SSH_CALL_PREFIX_D1}; ./$STARTSCRIPTS_SUBDIR/slave_newcave.sh ${SLAVENAME_SECOND} $@" &

    fi
done
