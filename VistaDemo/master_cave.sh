#!/bin/bash

export __GL_FSAA_MODE=5
#export __GL_SYNC_TO_VBLANK=1
#export __GL_SYNC_DISPLAY_DEVICE=CRT-1

if [ "$1" = "" ]; then
	echo "No demo executable specified!"
	exit -4	
else
	EXECUTABLE="$1/$1"
	PATHSCRIPT="$1/set_path_for_$1.sh"
fi

if [ ! -x $EXECUTABLE ];
then
	echo "Specified file $EXECUTABLE does not exist, or is not executable"
	exit -5
di
	
fi

if [ "$2" = "" ]; then
	echo "No cluster configuration given."
	exit -4	
fi

if [ -x $PATHSCRIPT ]; then
	source $PATHSCRIPT
fi
$EXECUTABLE -clustermaster $2 -vistaini vista_cave.ini -interactionini interaction_cave -displayini display_cave.ini -clusterini display_cave.ini




