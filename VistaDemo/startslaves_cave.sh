#!/bin/bash

# save the directory
DIR=`pwd`
echo Working directory: ${DIR}

if [ "$2" = "" ];
then
	echo Usage: startslaves_cave DEMO_NAME CLUSTER_CONFIG
	echo      DEMO_NAME is full name, e.g. 01KeyboardDemo
	echo      CLUSTER_CONFIG can be CaveSpace or HoloSpace
	exit -2
fi

if [ "$2" = "CaveSpace" ];
then

	echo start on caveclient01
	ssh 10.0.0.1 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 FrontWallLeft" &
	echo start on caveclient02
	ssh 10.0.0.2 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 FrontWallRight" &
	echo start on caveclient03
	ssh 10.0.0.3 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 LeftWallLeft" &
	echo start on caveclient04
	ssh 10.0.0.4 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 LeftWallRight" &
	echo start on caveclient05
	ssh 10.0.0.5 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 RightWallLeft" &
	echo start on caveclient06
	ssh 10.0.0.6 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 RightWallRight" &
	echo start on caveclient07
	ssh 10.0.0.7 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 BottomLeft" &
	echo start on caveclient08
	ssh 10.0.0.8 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 BottomRight" &

elif [ "$2" = "HoloSpace" ];
then

	echo start on caveclient01
	ssh 10.0.0.1 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 FrontWallLeft" &
	echo start on caveclient02
	ssh 10.0.0.2 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 FrontWallRight" &
	echo start on caveclient03
	ssh 10.0.0.3 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 LeftWallLeft" &
	echo start on caveclient04
	ssh 10.0.0.4 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 LeftWallRight" &
	echo start on caveclient05
	ssh 10.0.0.5 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 RightWallLeft" &
	echo start on caveclient06
	ssh 10.0.0.6 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 RightWallRight" &
	echo start on caveclient07
	ssh 10.0.0.7 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 BottomLeft" &
	echo start on caveclient08
	ssh 10.0.0.8 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 BottomRight" &
	echo start on caveclient09
	ssh 10.0.0.9 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 BackLeft" &
	echo start on caveclient10
	ssh 10.0.0.10 ". /etc/profile; cd ${DIR}; ${DIR}/slave_cave.sh $1 BackRight" &

else
	
	echo Unknown Cluster configuration "$2"
	exit -3
fi


