#!/bin/bash

# script to run an application with a specific System configuration
# Modify this to select the available system setups and provide a system
# setup's default cluster mode iff it runs in cluster mode. For each
# system setup, one can also provide a list of hostnames in order to
# determine the default system if none is specified as parameter.
# Furthermore, adjust the AppNames, wether or not to redirect the
# output of slaves to a file, and provide help text about
# custom command-line parameters

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
APLLICATION_NAME_RELEASE=AppName
APPLICATION_NAME_DEBUG=AppNameD


# for each config, you may optionally specifiy a hostname (as regex)
# if this script is executed without specifying a system config, the
# corresponding config is started by default
desktop=( 'vrdev.*' 'linuxgpud.*' )
newcave=( 'linuxgpum1.*' )
oldcave=( 'vrvis7' )

# choose a default of wether or not the slave output should be redirected
# to log files in the slavelogs subfolder. Can be overwritten wit -R / -NR
export REDIRECT_SLAVE_OUTPUT=true

# in this file, you can define custom parameters, which wioll be listed
# under APP_PARAMS in the help output
custom_usage()
{
	echo "                -D   --debug"	
	echo "                     runs all instances in debug mode"
	echo "                -DM  --debug_master"	
	echo "                     runs only the master instance in debug mode (ClusterMode only)"
	echo "                -DS  --debug_slave"	
	echo "                     run only the slave instances in debug mode (ClusterMode only)"
	echo "                -TV  --totalview"	
	echo "                     starts debugging with totalview (only Master if ClusterMode)"
	echo "                -VG  --valgrind"	
	echo "                     starts profiling with valgrind (only Master if ClusterMode)"
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
	echo "                     Cluster Configuration (e.g. Holospace, Frontwall) to use instead"
	echo "                     of the default (ClusterMode only)"
	echo "                -R   --redirect-slave-output"
	echo "                     redirect slave output to file in slavelogs subdir"
	echo "                -NR  --dont-redirect-slave-output"
	echo "                     don't redirect slave output to file"
	echo "                -K   --kill"
	echo "                     don't start the application, only execute the kill script"
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
	if [ "${INPUTSYSTEM_LOWERCASE}" == "$TESTSYSTEM" ];	then	
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
	if [ "$1" == "-R" -o "$1" == "--redirect-slave-output" ]; then
		export REDIRECT_SLAVE_OUTPUT=true
		shift
	elif [ "$1" == "-NR" -o "$1" == "--dont_redirect-slave-output" ]; then
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
			killall $APLLICATION_NAME_RELEASE &
			killall $APLLICATION_NAME_DEBUG &
			exit
		fi
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
	
	./startscripts/start_$CHOSENSYSTEM.sh $@
	
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
	./startscripts/startslaves_$CHOSENSYSTEM.sh $CLUSTER_CONFIGURATION $@

	sleep 5

	echo ""
	echo "starting master"
	./startscripts/master_$CHOSENSYSTEM.sh $CLUSTER_CONFIGURATION $@

	sleep 1

	echo ""
	echo "executing kill script"
	./startscripts/kill_$CHOSENSYSTEM.sh
	
fi

#if [ -f "run_$CHOSENSYSTEM.sh" ]; then
#	./run_$CHOSENSYSTEM.sh $@
#elif [ -f "startscripts/start_$CHOSENSYSTEM.sh" ]; then
#	./startscripts/start_$CHOSENSYSTEM.sh $@
#else
#	echo "run.sh - Config \"$CHOSENSYSTEM\" selected, but script run_$CHOSENSYSTEM.sh or start_$CHOSENSYSTEM.sh not found"
#	exit -1
#fi


