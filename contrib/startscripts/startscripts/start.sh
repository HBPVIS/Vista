#!/bin/bash

# script to start the actual application without system-specific options
# Modify this to math the actual executable name, and choose/modify the 
# parameters that should be parsed before the application
# Currently, the first parameters are parser to determine the execution
# mode, i.e. wether to start as release (default), or use some kind of
# debugging/profiling, ...

source ./bin/set_path_for_APPNAME.sh

EXECUTABLE="./AppName"

while [ ! "$1" == "" ]; do

	if [ "$1" == "-D" -o "$1" == "--debug" ]; then
		shift
		EXECUTABLE="./AppName"
	elif [ "$1" == "-DM" -o "$1" == "--debug_master" ]; then
		shift
		if [ ! "$IS_SLAVE" == "true" ]; then
			EXECUTABLE="./AppNameD"
		fi
	elif [ "$1" == "-DS" -o "$1" == "--debug_slave" ]; then
		shift
		if [ "$IS_SLAVE" == "true" ]; then
			EXECUTABLE="./AppNameD"
		fi
	elif [ "$1" == "-TV" -o "$1" == "--totalview" ]; then
		shift
		if [ ! "$IS_SLAVE" == "true" ]; then
			EXECUTABLE="totalview ./AppNameD -a"
		fi
	elif [ "$1" == "-VG" -o "$1" == "--valgrind" ]; then
		shift
		if [ ! "$IS_SLAVE" == "true" ]; then
			EXECUTABLE="valgrind ./AppNameD"
		fi	
	else
		# last prefix parameter, rest should be for the app
		break
	fi
done

$EXECUTABLE $@


