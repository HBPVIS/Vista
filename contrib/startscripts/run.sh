#!/bin/bash

# list available system setups here
# note that standalone versions need to provide a script
# start_SYSTEM.sh, Cluster-systems a run_SYSTEM.sh.
# These scripts are to be located in a startscripts subfolder
SYSTEMSETUPS=( desktop newcave oldcave )

# cluster modes have to define a DEFAULT_CLUSTERCONFIG
# is a syystem is not listed here, it will be started as standalone
newcave_DEFAULT_CLUSTERCONFIG=HoloSpace
oldcave_DEFAULT_CLUSTERCONFIG=HoloSpace

# list the application names here, in order to allow killscripts
# to ensure the apps are properly terminated
export APPLICATION_NAME_RELEASE=YOUR_APPNAME
export APPLICATION_NAME_DEBUG=YOUR_APPNAMED

# specifies where the slave node configuration file is located
# usually, you should use the common one on /home/vrsw/gpucluster, 
# which hopefully contains a stable setting
export SLAVENODES_CONFIGURATION_FILE="/home/vrsw/gpucluster/slavenodes_ipconfig_newcave.sh"

# specifies the number of seconds to wait between starting the slaves and the master
WAIT_AFTER_SLAVE_START=5


# for each config, you may optionally specifiy a hostname (as regex)
# if this script is executed without specifying a system config, the
# corresponding config is started by default
desktop=( 'vrdev.*' 'linuxgpud.*' )
newcave=( 'linuxgpum1.*' )
oldcave=( 'vrvis7' )

# choose a default of wether or not the slave output should be redirected
# to log files in the slavelogs subfolder. Can be overwritten wit -R / -NR
export REDIRECT_SLAVE_OUTPUT=true

# this is a list of additional parameters. in general, all parameters that
# are not parsed by the run.sh file are forwarded to the next scripts in
# the same order. to add additional parameters, add them to the following
# two vars. parameters in ADDITIONAL_PARAMS_PRE will passed in front of any
# additional parameters to this scripts, those in ADDITIONAL_PARAMS_POST after
ADDITIONAL_PARAMS_PRE=
ADDITIONAL_PARAMS_POST=

# in this file, you can define custom parameters, which wioll be listed
# under APP_PARAMS in the help output
custom_usage()
{
    echo ""
}

########################################
# Don't edit below this line           #
# (unless you know what you're doing)  #
########################################

usage()
{
    echo "$0 [System] [RUN_OPTIONS] [APP_PARAMS]"
    echo "runs ${APLLICATION_NAME_RELEASE} as the desired system configuration"
    echo "         System:  specifies the System setup config of the app,"
    echo "                  can be ${SYSTEMSETUPS[@]}"
    echo "                  if no System is specified, this script tries to"
    echo "                  determine it from the hostname"
    echo "    RUN_OPTIONS:  advanced configuration of the run script"
    echo "                -c ClusterConfig"
	echo "                --cluster-configc ClusterConfig"
    echo "                     Cluster Configuration (e.g. Holospace, Frontwall) to use instead"
    echo "                     of the default (ClusterMode only)"
    echo "                -x n1 n2 ..."
	echo "                --exclude-slaves n1 n2 ..."
    echo "                     excludes the listed slave nodes"
    echo "                -R"
	echo "                --redirect-slave-output"
    echo "                     redirect slave output to file in slavelogs subdir"
    echo "                -NR"
	echo "                --dont-redirect-slave-output"
    echo "                     don't redirect slave output to file"
    echo "                -K"
	echo "                --kill"
    echo "                     don't start the application, only execute the kill script"
	echo "                -SC file"
	echo "                --slave-config-file file"
	echo "                     specify an override for the slave configuration file"
    echo "      APP_PARAMS:  Application-specific parameters"
    custom_usage
}

CLUSTER_CONFIGURATION=
CHOSENSYSTEM=

if [ "$1" == "-h" -o "$1" == "--help" ]; then
    usage
    exit
fi

# check if the first parameter describes a system
INPUTSYSTEM_LOWERCASE=`echo $1  | tr '[A-Z]' '[a-z]'`
for TESTSYSTEM in ${SYSTEMSETUPS[@]}; do
    if [ "${INPUTSYSTEM_LOWERCASE}" == "$TESTSYSTEM" ];    then
        shift
        CHOSENSYSTEM=$TESTSYSTEM
        break
    fi
done

# if no system was found from the first parameter, check there
# is a default config for this hostname
if [ "$CHOSENSYSTEM" == "" ]; then
    for TESTSYSTEM in ${SYSTEMSETUPS[@]}; do
        HOST_PATTERN_LIST="$TESTSYSTEM[@]"
        for HOST_PATTERN in ${!HOST_PATTERN_LIST}; do
            if [[ $HOSTNAME =~ $HOST_PATTERN ]]; then
                CHOSENSYSTEM=$TESTSYSTEM
            fi
        done
    done
fi

if [ "$CHOSENSYSTEM" == "" ]; then
    echo "run.sh - no suitable system found"
    usage
    exit -1
fi

# parse parameters

while [ ! "$1" == "" ]; do
InsideD
    if [ "$1" == "-R" -o "$1" == "--redirect-slave-output" ]; then
        export REDIRECT_SLAVE_OUTPUT=true
        shift
    elif [ "$1" == "-NR" -o "$1" == "--dont-redirect-slave-output" ]; then
        export REDIRECT_SLAVE_OUTPUT=false
        shift
    elif [ "$1" == "-c" -o "$1" == "--cluster-config" ]; then
            CLUSTER_CONFIGURATION=$2
            shift
            shift
    elif [ "$1" == "-K" -o "$1" == "--kill" ]; then
        if [ -f "startscripts/kill_$CHOSENSYSTEM.sh" ]; then
            echo "executing kill script"
            ./startscripts/kill_$CHOSENSYSTEM.sh
            exit
        else
            killall $APPLICATION_NAME_RELEASE &
            killall $APPLICATION_NAME_DEBUG &
            exit
        fi
    elif [ "$1" == "-x" -o "$1" == "--exclude-slaves" ]; then
        shift
        while [[ "$1" =~ ^[0-9]+$ ]]; do
            if [ "$1" -gt 9 ]; then
                EXCLUDE_VAR_NAME=SKIP_SLAVE$1
            else
                EXCLUDE_VAR_NAME=SKIP_SLAVE0$1
            fi
            eval export $EXCLUDE_VAR_NAME=1
            shift
        done
	elif [ "$1" == "-SC" -o "$1" == "--slave-config" ]; then
        shift
        export SLAVENODES_CONFIGURATION_FILE=$1
		echo "overriding slave node configuration file to $SLAVENODES_CONFIGURATION_FILE"
		shift
    else
        # unknown parameters --> end of run.sh params
        break
    fi
done

DEFAULT_CLUSTERCONFIG_VAR=${CHOSENSYSTEM}_DEFAULT_CLUSTERCONFIG
DEFAULT_CLUSTERCONFIG=${!DEFAULT_CLUSTERCONFIG_VAR}

if [ "${DEFAULT_CLUSTERCONFIG}" == "" ]; then
    # no DEFAULT_CLUSTERCONFIG exists, thus we run it in standalone mode

    if [ ! -f "startscripts/start_$CHOSENSYSTEM.sh" ]; then
        echo "run.sh - Config \"$CHOSENSYSTEM\" selected for Standalone, but script start_$CHOSENSYSTEM.sh not found"
        exit -1
    fi

    ./startscripts/start_$CHOSENSYSTEM.sh $ADDITIONAL_PARAMS_PRE $@ $ADDITIONAL_PARAMS_POST

else

    if [ ! -f "startscripts/master_$CHOSENSYSTEM.sh" ] || [ ! -f "startscripts/slave_$CHOSENSYSTEM.sh" ] || [ ! -f "startscripts/startslaves_$CHOSENSYSTEM.sh" ]; then
        echo "run.sh - Config \"$CHOSENSYSTEM\" selected for ClusterMode, but not all required scripts (startslaves_$CHOSENSYSTEM.sh, master_$CHOSENSYSTEM.sh, slave_$CHOSENSYSTEM.sh) exist"
        exit -1
    fi

    if [ "$CLUSTER_CONFIGURATION" == "" ]; then
        export CLUSTER_CONFIGURATION=$DEFAULT_CLUSTERCONFIG
    fi


    echo ""
    echo "starting slaves"
    ./startscripts/startslaves_${CHOSENSYSTEM}.sh $CLUSTER_CONFIGURATION $ADDITIONAL_PARAMS_PRE $@ $ADDITIONAL_PARAMS_POST

    sleep $WAIT_AFTER_SLAVE_START

    echo ""
    echo "starting master"
    ./startscripts/master_$CHOSENSYSTEM.sh $CLUSTER_CONFIGURATION $ADDITIONAL_PARAMS_PRE $@ $ADDITIONAL_PARAMS_POST

    sleep 1

    echo ""
    if [ -f "startscripts/kill_$CHOSENSYSTEM.sh" ]; then
        echo "executing kill script"
        ./startscripts/kill_$CHOSENSYSTEM.sh >/dev/null 2>/dev/null
    fi

fi


