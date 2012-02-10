#!/bin/bash

export __GL_FSAA_MODE=5
#export __GL_SYNC_TO_VBLANK=1
#export __GL_SYNC_DISPLAY_DEVICE=CRT-1

if [ "$1" = "" ]; then
	echo "No demo executable specified!"
	exit -4	
else
	EXECUTABLE="./$1"
	PATHSCRIPT="set_path_for_$1.sh"
fi

if [ ! -d $1 ];
then
	echo "Specified dir $1 does not exist"
	exit -5
fi

cd $1

if [ ! -x $1 ];
then
	echo "Specified file $EXECUTABLE does not exist, or is not executable"
	exit -5
fi
	
if [ "$2" = "" ]; then
	echo "No cluster configuration given."
	exit -4	
fi

if [ -x $PATHSCRIPT ]; then
	source $PATHSCRIPT
fi
$EXECUTABLE -clustermaster $2 -vistaini vista_cave.ini -interactionini interaction_cave.ini -displayini display_cave.ini -clusterini display_cave.ini




