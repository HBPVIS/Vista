#!/usr/bin/python
# -*- coding: utf-8 -*-

import math
import os
import stat

W = 5.25
H = 3.3

sCaveName = "newcave"

HorOverlap = 0.51785
VertOverlap = 0.314285
FloorHorOverlap = 0.375
FloorVertOverlap = 0.5625

aWalls       = [ "FloorFront",         "FloorBack",          "Left",              "Front",              "Right",              "Rear" ]
aWallCenters = [ (0.0, 0.0, -0.25*W),  (0.0, 0.0, +0.25*W),  (-.5*W, .5*H, 0.0),  (0.0, .5*H, -0.5*W),  (0.5*W, 0.5*H, 0.0),  (0.0, 0.5*H, 0.5*W) ]
aWallNormals = [ (0.0, 1.0, 0.0),      (0.0, 1.0, 0.0),      (1.0, 0.0, 0.0),     (0.0, 0.0, 1.0),      (-1.0, 0.0, 0.0),     (0.0, 0.0, -1.0) ]
aWallUps     = [ (0.0, 0.0, -1.0),     (0.0, 0.0, -1.0),     (0.0, 1.0, 0.0),     (0.0, 1.0, 0.0),      (0.0, 1.0, 0.0),      (0.0, 1.0, 0.0) ]
aIsFloor     = [ True,                 True,                 False,               False,                False,                False ]
aRows = [ "T", "B" ]
aColumns = [ "L", "R" ]
aEyes = [ "RE", "LE" ]

sBCAddress = "10.0.1.255"
sZeroMQAddress = "epgm://eth1;224.1.1.29"
sZeroMQPorts = "19000-19999"

aConfigs = [
	( "FrontWall", [3] ),
	( "HoloSpace", [0, 1, 2, 3, 4, 5] ),
	( "CaveSpace", [0, 1, 2, 3, 4] ),
	( "Angle", [2, 3] )
]

nCount = len(aWalls) * len(aRows) * len(aColumns) * len(aEyes)

aSlaveIPsInfiniband = [ "192.168.88.20", "192.168.88.22", "192.168.88.23", "192.168.88.24", "192.168.88.25", "192.168.88.26", "192.168.88.27", "192.168.88.28", "192.168.88.29", "192.168.88.30", "192.168.88.31", "192.168.88.32", "192.168.88.33", "192.168.88.34", "192.168.88.35", "192.168.88.36", "192.168.88.37", "192.168.88.38", "192.168.88.39", "192.168.88.40", "192.168.88.41", "192.168.88.42", "192.168.88.43", "192.168.88.44", "192.168.88.45", "192.168.88.46", "192.168.88.47" ]
aSlaveIPSClusterNet = [ "134.61.201.192", "134.61.201.193", "134.61.201.194", "134.61.201.195", "134.61.201.196", "134.61.201.197", "134.61.201.198", "134.61.201.199", "134.61.201.200", "134.61.201.201", "134.61.201.202", "134.61.201.203", "134.61.201.204", "134.61.201.205", "134.61.201.206", "134.61.201.207", "134.61.201.208", "134.61.201.209", "134.61.201.210", "134.61.201.211", "134.61.201.212", "134.61.201.213", "134.61.201.214", "134.61.201.215", "134.61.201.217", "134.61.201.218", "134.61.201.219" ]

sFilename = "display_" + sCaveName + ".ini"
oFile  = open( sFilename, 'w' )


def WriteVec( aVec ):
	oFile.write( str( aVec[0] ) + ", " + str( aVec[1] ) + ", " + str( aVec[2] ) + "\n" )

def GetIPForSlave( nNumber ):
	nNumber = int(0.5*nNumber+0.5)
	#we have to switch row and column for the floor, i.e. the first 8 slaves
	if nNumber == 2:
		nNumber = 3
	elif nNumber == 3:
		nNumber = 2
	elif nNumber == 6:
		nNumber = 7
	elif nNumber == 7:
		nNumber = 6

	return "10.0.1." + str(nNumber)
	#return aSlaveIPSClusterNet[nNumber]
	#return aSlaveIPsInfiniband[nNumber]
		
def PrintCenterAndNormal( nWallId ):
	oFile.write( "PROJ_PLANE_MIDPOINT = " )
	WriteVec( aWallCenters[nWallId] )
	oFile.write( "PROJ_PLANE_NORMAL   = " )
	WriteVec( aWallNormals[nWallId] )
	oFile.write( "PROJ_PLANE_UP       = " )
	WriteVec( aWallUps[nWallId] )
	
def PrintProjExtents( nWallId, nRow, nColumn ):
	aExtents = [ 0, 0, 0, 0 ]
	if( aIsFloor[nWallId] == True ):
		if( nColumn == 0 ):
			# left			
			aExtents[0] = -0.5*W
			aExtents[1] = +FloorHorOverlap
		else:
			#right
			aExtents[0] = -FloorHorOverlap
			aExtents[1] = 0.5*W			
			
		if( nRow == 0 ):
			# top
			aExtents[2] = -FloorVertOverlap
			aExtents[3] = 0.5*0.5*W
		else:
			#bottom
			aExtents[2] = -0.5*0.5*W
			aExtents[3] = +FloorVertOverlap
		# we don't actuall have rows and colums
		#nID = 2 * nRow + nColumn	
		#nProjectorWidth = ( W + 3 * 2 * FloorHorOverlap ) / 4

		#aExtents[0] = -0.5 * W + nID * nProjectorWidth
		#aExtents[0] = aExtents[0] - nID * FloorHorOverlap;
		#aExtents[1] = aExtents[0] + nProjectorWidth
		
		#if( nWallId == 0 ):
		#	aExtents[2] = -0.5 * W
		#	aExtents[3] = FloorVertOverlap
		#else:
		#	aExtents[2] = -FloorVertOverlap
		#	aExtents[3] = 0.5 * W
	else:
		if( nColumn == 0 ):
			# left
			aExtents[0] = -0.5*W
			aExtents[1] = +HorOverlap
		else:
			#right
			aExtents[0] = -HorOverlap
			aExtents[1] = 0.5*W
			
			
		if( nRow == 0 ):
			#top
			aExtents[2] = -VertOverlap
			aExtents[3] = 0.5*H
		else:
			#bottom
			aExtents[2] = -0.5*H
			aExtents[3] = +VertOverlap		
	
	oFile.write( "PROJ_PLANE_EXTENTS  = "
				+ str( aExtents[0] ) + ", "
				+ str( aExtents[1] ) + ", "
				+ str( aExtents[2] ) + ", "
				+ str( aExtents[3] ) + "\n" )
	
		
def GetNameFor( nWallId, nRowId, nColumnId, nEyeId ):
#	if( nWallId < 2 ):
#		return aWalls[nWallId] + "_" + str( 2*nRowId + nColumnId ) + "_" + aEyes[nEyeId]
#	else:
		return aWalls[nWallId] + "_" + aRows[nRowId] + aColumns[nColumnId] + "_" + aEyes[nEyeId]
	
def GetIdForConfig( nWallId, nRowId, nColumnId, nEyeId ):
	nId = nWallId
	nId = nId * len(aRows) + nRowId
	nId = nId * len(aColumns) + nColumnId
	nId = nId * len(aEyes) + nEyeId
	return nId + 1
	
def CreateSlaveSystemSection( nWallId, nRowId, nColumnId, nEyeId ):
	sName = GetNameFor( nWallId, nRowId, nColumnId, nEyeId )
	sNameUpper = str.upper( sName )
	nId = GetIdForConfig( nWallId, nRowId, nColumnId, nEyeId )
	nSlavePort = 18970 + nEyeId
	nAckPort = 18980 + nEyeId
	nSyncPort = 18990 + nEyeId
	nPortRangeStart = 18700 + 100 * nEyeId
	nPortRangeEnd = 18799 + 100 * nEyeId
	
	oFile.write( "[" + sName + "]\n" )
	if( nId < 10 ):
		oFile.write( "NAME                = Slave0" + str(nId) + "\n" )
	else:
		oFile.write( "NAME                = Slave" + str(nId) + "\n" )
	oFile.write( "DISPLAYSYSTEMS      = DISPLAY_" + sNameUpper + "\n" )
	oFile.write( "SLAVEIP             = " + GetIPForSlave( nId ) + "\n" )
	oFile.write( "SLAVEPORT           = " + str(nSlavePort) + "\n" )
	oFile.write( "ACKPORT             = " + str(nAckPort) + "       # for old master/slave\n" )
	oFile.write( "SYNC                = TRUE        # for old master/slave\n" )
	oFile.write( "SYNCIP              = " + sBCAddress  +"  # for old master/slave\n" )
	oFile.write( "SYNCPORT            = " + str(nSyncPort) + "       # for old master/slave\n" )
	oFile.write( "DOGLFINISH          = TRUE\n" )
	oFile.write( "BYTESWAP            = FALSE\n" )
	oFile.write( "OUTPUT              = OUTSTREAMS_SLAVE\n" )
	oFile.write( "FREEPORTS           = " + str(nPortRangeStart) + "-" + str(nPortRangeEnd) + "\n" )
	oFile.write( "\n\n" )
	
def CreateMasterSystemSection():
	nConfigCount = 0
	for Config in aConfigs:
		sName = Config[0]
		aWallConfigs = Config[1]
		
		oFile.write( "[" + sName + "]\n" )
		oFile.write( "NAME                = " + sName + "\n" )
		oFile.write( "DISPLAYSYSTEMS      = DISPLAY_MASTER\n" )
		oFile.write( "BYTESWAP            = TRUE\n" )
		oFile.write( "SYNC                = TRUE            # for old master/slave\n" )
		oFile.write( "SYNCIP              = " + sBCAddress + "      # for old master/slave\n" )
		oFile.write( "SYNCPORTS           = 18990, 18991    # for old master/slave\n" )
		oFile.write( "SLAVES              = " )
		for nWall in aWallConfigs:
			for nRow in range(len(aRows)):
				for nColumn in range(len(aColumns)):
					for nEye in range(len(aEyes)):
						sName = GetNameFor( nWall, nRow, nColumn, nEye )
						oFile.write( sName )
						if nWall == aWallConfigs[len(aWallConfigs)-1] and nRow+1 == len(aRows) and nColumn+1 == len(aColumns) and nEye+1 == len(aEyes):
							oFile.write( "\n" )
						else:
							oFile.write( ", " )
		oFile.write( "BROADCASTIP          = " + sBCAddress + "\n" )
		oFile.write( "BROADCASTPORTS       = 15000-15999\n" )
		oFile.write( "ZEROMQ_ADDRESS       = " + sZeroMQAddress + "\n" )
		oFile.write( "ZEROMQ_PORTS         = " + sZeroMQPorts + "\n" )
		oFile.write( "BARRIERTYPE          = ${BARRIER_MODE}\n" )
		oFile.write( "SWAPSYNCTYPE         = ${SWAPSYNC_MODE}\n" )
		oFile.write( "DATASYNCTYPE         = ${DATASYNC_MODE}\n" )
		oFile.write( "OUTPUT               = OUTSTREAMS_MASTER\n" )
		oFile.write( "\n\n" )
		
def CreateMasterDisplaySystem():
	oFile.write( "########################################\n" )
	oFile.write( "## MASTER DISPLAY CONFIG\n" )
	oFile.write( "########################################\n" )
	oFile.write( "[DISPLAY_MASTER]\n" )
	oFile.write( "NAME                = MAIN\n" )
	oFile.write( "REFERENCE_FRAME     = INITIAL_REFERENCE_FRAME\n" )
	oFile.write( "VIEWPORTS           = VIEWPORT_MASTER\n" )
	oFile.write( "VIEWER_POSITION     = ${INIT_VIEWER_POS}\n" )
	oFile.write( "VIEWER_ORIENTATION  = ${INIT_VIEWER_ORI}\n" )
	oFile.write( "LEFT_EYE_OFFSET     = ${LEFT_EYE_OFFSET}\n" )
	oFile.write( "RIGHT_EYE_OFFSET    = ${RIGHT_EYE_OFFSET}\n" )
	oFile.write( "WINDOWINGTOOLKIT    = ${WINDOWINGTK}\n" )
	oFile.write( "HMD_MODE            = TRUE\n" )
	oFile.write( "\n" )
	oFile.write( "[VIEWPORT_MASTER]\n" )
	oFile.write( "NAME                = MAIN_VIEWPORT\n" )
	oFile.write( "PROJECTION          = PROJECTION_MASTER\n" )
	oFile.write( "WINDOW              = WINDOW_MASTER\n" )
	oFile.write( "\n" )
	oFile.write( "[PROJECTION_MASTER]\n" )
	oFile.write( "NAME                = MAIN_PROJECTION\n" )
	oFile.write( "PROJ_PLANE_MIDPOINT = 0.0, 0.0, -1.0\n" )
	oFile.write( "PROJ_PLANE_NORMAL   = 0.0, 0.0, 1.0\n" )
	oFile.write( "PROJ_PLANE_UP       = 0.0, 1.0, 0.0\n" )
	oFile.write( "PROJ_PLANE_EXTENTS  = -1,1, -0.75,0.75\n" )
	oFile.write( "CLIPPING_RANGE      = ${CLIPPING_RANGE}\n" )
	oFile.write( "STEREO_MODE         = MONO\n" )
	oFile.write( "\n" )
	oFile.write( "[WINDOW_MASTER]\n" )
	oFile.write( "NAME                = MAIN_WINDOW\n" )
	oFile.write( "POSITION            = 20, 20\n" )
	oFile.write( "SIZE                = 1024, 768\n" )
	oFile.write( "STEREO              = FALSE\n" )
	oFile.write( "VSYNC               = ${MASTER_VSYNC_MODE}\n" )
	oFile.write( "\n\n" )
	
def CreateSlaveDisplaySystem( nWallId, nRowId, nColumnId, nEyeId ):
	sName = GetNameFor( nWallId, nRowId, nColumnId, nEyeId )
	sNameUpper = str.upper( sName )
	nId = GetIdForConfig( nWallId, nRowId, nColumnId, nEyeId )
	
	oFile.write( "########################################\n" )
	oFile.write( "## " + sNameUpper + " DISPLAY CONFIG\n" )
	oFile.write( "########################################\n" )
	oFile.write( "[DISPLAY_" + sNameUpper + "]\n" )
	oFile.write( "NAME                = MAIN\n" )
	oFile.write( "REFERENCE_FRAME     = INITIAL_REFERENCE_FRAME\n" )
	oFile.write( "VIEWPORTS           = VIEWPORT_" + sNameUpper + "\n" )
	oFile.write( "VIEWER_POSITION     = ${INIT_VIEWER_POS}\n" )
	oFile.write( "VIEWER_ORIENTATION  = ${INIT_VIEWER_ORI}\n" )
	oFile.write( "LEFT_EYE_OFFSET     = ${LEFT_EYE_OFFSET}\n" )
	oFile.write( "RIGHT_EYE_OFFSET    = ${RIGHT_EYE_OFFSET}\n" )
	oFile.write( "WINDOWINGTOOLKIT    = ${WINDOWINGTK}\n" )
	oFile.write( "\n" )
	oFile.write( "[VIEWPORT_" + sNameUpper + "]\n" )
	oFile.write( "NAME                = MAIN_VIEWPORT\n" )
	oFile.write( "PROJECTION          = PROJECTION_" + sNameUpper + "\n" )
	oFile.write( "WINDOW              = WINDOW_SLAVES\n" )
	oFile.write( "\n" )
	oFile.write( "[PROJECTION_" + sNameUpper + "]\n" )
	oFile.write( "NAME                = MAIN_PROJECTION\n" )
	PrintCenterAndNormal( nWallId )
	PrintProjExtents( nWallId, nRow, nColumn )
	oFile.write( "CLIPPING_RANGE      = ${CLIPPING_RANGE}\n" )
	if nEyeId == 1:
		oFile.write( "STEREO_MODE         = LEFT_EYE\n" )
	else:
		oFile.write( "STEREO_MODE         = RIGHT_EYE\n" )
	oFile.write( "\n\n" )
	
def CreateSlaveWindow():
	oFile.write( "########################################\n" )
	oFile.write( "## COMMON SLAVE CONFIG\n" )
	oFile.write( "########################################\n" )
	oFile.write( "\n" )
	oFile.write( "[WINDOW_SLAVES]\n" )
	oFile.write( "NAME                = MAIN_WINDOW\n" )
	oFile.write( "POSITION            = 0, 0\n" )
	oFile.write( "SIZE                = 1920, 1200\n" )
	oFile.write( "STEREO              = FALSE\n" )
	oFile.write( "VSYNC               = ${SLAVE_VSYNC_MODE}\n" )
	oFile.write( "\n\n" )

def CreateReferenceFrame():
	oFile.write( "########################################\n" )
	oFile.write( "## REFERENCE FRAME FOR ALL DISPLAY SYSTEMS\n" )
	oFile.write( "########################################\n" )
	oFile.write( "\n" )
	oFile.write( "[INITIAL_REFERENCE_FRAME]\n" )
	oFile.write( "TRANSLATION         = 0, 0, 0\n" )
	oFile.write( "ROTATION            = 0, 0, 0, 1\n" )
	oFile.write( "\n\n" )

def CreateFileScopeVariables():
	oFile.write( "########################################\n" )
	oFile.write( "## FILE-SCOPE VARIABLES\n" )
	oFile.write( "########################################\n" )
	oFile.write( "\n" )
	oFile.write( "[FILE_VARIABLES]\n" )
	oFile.write( "LEFT_EYE_OFFSET     = -0.03, 0, 0\n" )
	oFile.write( "RIGHT_EYE_OFFSET    = +0.03, 0, 0\n" )
	oFile.write( "INIT_VIEWER_POS     = 0, 1.6, 0\n" )
	oFile.write( "INIT_VIEWER_ORI     = 0, 0, 0, 1\n" )
	oFile.write( "MASTER_VSYNC_MODE   = FALSE\n" )
	oFile.write( "SLAVE_VSYNC_MODE    = TRUE\n" )
	oFile.write( "CLIPPING_RANGE      = 0.05, 1000.0\n" )
	oFile.write( "DATASYNC_MODE       = ZEROMQ\n" )
	oFile.write( "BARRIER_MODE        = BROADCAST\n" )
	oFile.write( "SWAPSYNC_MODE       = BROADCAST\n" )	
	oFile.write( "WINDOWINGTK         = GLUT\n" )
	oFile.write( "\n\n" )
	
def CreateStreamDefintions():
	oFile.write( "########################################\n" )
	oFile.write( "## OUT-STREAM SPECIFICATION\n" )
	oFile.write( "########################################\n" )
	oFile.write( "\n" )
	oFile.write( "[OUTSTREAMS_MASTER]\n" )
	oFile.write( "THREADSAFE          = TRUE\n" )
	oFile.write( "OUT                 = COUT\n" )
	oFile.write( "WARN                = COLOR( BLACK, YELLOW )\n" )
	oFile.write( "ERR                 = COLOR( BLACK, RED )\n" )
	oFile.write( "DEBUG               = BUILDTYPE( OUT, NULL )\n" )
	oFile.write( "\n" )
	oFile.write( "[OUTSTREAMS_SLAVE]\n" )
	#oFile.write( "THREADSAFE          = TRUE\n" )
	#oFile.write( "OUT                 = COUT\n" )
	#oFile.write( "WARN                = COLOR( BLACK, YELLOW )\n" )
	#oFile.write( "ERR                 = COLOR( BLACK, RED )\n" )
	#oFile.write( "DEBUG               = BUILDTYPE( OUT, NULL )\n" )
	oFile.write( "THREADSAFE          = TRUE\n" )
	oFile.write( "OUT                 = FILE( slavelogs/log_, ADD_NODENAME )\n" )
	oFile.write( "WARN                = SPLIT( OUT, PREFIX( COLOR( BLACK, YELLOW ), \"\", ADD_NODENAME ) )\n" )
	oFile.write( "ERR                 = SPLIT( OUT, PREFIX( COLOR( BLACK, RED ), \"\", ADD_NODENAME ) )\n" )
	oFile.write( "DEBUG               = BUILDTYPE( OUT, NULL )\n" )
	oFile.write( "\n\n" )
		

CreateFileScopeVariables()
oFile.write( "########################################\n" )
oFile.write( "## MASTER CONFIGURATIONS\n" )
oFile.write( "########################################\n" )
CreateMasterSystemSection()
oFile.write( "\n" )
oFile.write( "########################################\n" )
oFile.write( "## SLAVE CONFIGURATIONS\n" )
oFile.write( "########################################\n" )
for nWall in range(len(aWalls)):
	for nRow in range(len(aRows)):
		for nColumn in range(len(aColumns)):
			for nEye in range(len(aEyes)):
				CreateSlaveSystemSection( nWall, nRow, nColumn, nEye )
oFile.write( "\n" )
CreateMasterDisplaySystem()
for nWall in range(len(aWalls)):
	for nRow in range(len(aRows)):
		for nColumn in range(len(aColumns)):
			for nEye in range(len(aEyes)):
				CreateSlaveDisplaySystem( nWall, nRow, nColumn, nEye )
CreateSlaveWindow()
CreateReferenceFrame()
CreateStreamDefintions()



# creating the start script
sStartscriptFilename = "startslaves_" + sCaveName + ".sh"
oStartFile  = open( sStartscriptFilename, 'w' )

oStartFile.write( "#!/bin/bash\n\n" )
oStartFile.write( "\n" )
oStartFile.write( "if [ \"$1\" = \"\" ];\n" )
oStartFile.write( "then\n" )
oStartFile.write( "\techo 'parameter missing - specify a cluster configuration ( " )
for Config in aConfigs:
	oStartFile.write( str( Config[0] ) + " " )
oStartFile.write( ")'\n" )
oStartFile.write( "\texit\n" )
oStartFile.write( "fi\n" )
oStartFile.write( "\n" )
oStartFile.write( "if [ \"$2\" = \"\" ];\n" )
oStartFile.write( "then\n" )
oStartFile.write( "\tPRIORITY=\"\"\n" )
oStartFile.write( "else\n" )
oStartFile.write( "\techo using priority $2\n" )
oStartFile.write( "\tPRIORITY=$2\n" )
oStartFile.write( "fi\n" )
oStartFile.write( "\n" )
oStartFile.write( "INPUTCONFIG=${1,,}\n" )
oStartFile.write( "CHOSENCONFIG=\"\"\n" )
oStartFile.write( "\n" )
oStartFile.write( "CONFIGS=( " )
for Config in aConfigs:
	oStartFile.write( str( Config[0] ) + " " )
oStartFile.write( " )\n" )
oStartFile.write( "\n" )
for Config in aConfigs:
	oStartFile.write( Config[0] + "=( " )
	for nScreenNum in Config[1]:
		oStartFile.write( str( 4*nScreenNum + 1) + " " + str( 4*nScreenNum + 2 ) + " " + str( 4*nScreenNum + 3 ) + " " + str( 4*nScreenNum + 4 ) + " " )
	oStartFile.write( ")\n" )
oStartFile.write( "\n" )
oStartFile.write( "SLAVENAMES=( " )
for nWallId in range(len(aWalls)):
	for nRowId in range(len(aRows)):
		for nColumnId in range(len(aColumns)):
			for nEyeId in range(len(aEyes)):
				if aIsFloor[nWallId]:
					oStartFile.write( GetNameFor( nWallId, nColumnId, nRowId, nEyeId ) + " " )
				else:
					oStartFile.write( GetNameFor( nWallId, nRowId, nColumnId, nEyeId ) + " " )
oStartFile.write( " )\n" )
oStartFile.write( "\n" )
oStartFile.write( "for TESTCONFIG in ${CONFIGS[@]}\n" )
oStartFile.write( "do\t\n" )
oStartFile.write( "\tif [ \"${TESTCONFIG,,}\" == \"$INPUTCONFIG\" ];\n" )
oStartFile.write( "\tthen\n" )
oStartFile.write( "\t\tCHOSENCONFIG=$TESTCONFIG\n" )
oStartFile.write( "\t\tbreak\n" )
oStartFile.write( "\tfi\n" )
oStartFile.write( "done\n" )
oStartFile.write( "\n" )
oStartFile.write( "if [ \"$CHOSENCONFIG\" == \"\" ];\n" )
oStartFile.write( "then\n" )
oStartFile.write( "\techo \"Provided Config does not exist\"\n" )
oStartFile.write( "\techo \"Configs are: ${CONFIGS[@]}\"\n" )
oStartFile.write( "\texit\n" )
oStartFile.write( "else\n" )
oStartFile.write( "\techo \"Starting config $CHOSENCONFIG\"\n" )
oStartFile.write( "fi\n" )
oStartFile.write( "\n" )
oStartFile.write( "SLAVELIST=\"$CHOSENCONFIG[@]\"\n" )
oStartFile.write( "\n" )
oStartFile.write( "DIR=`pwd`\n" )
oStartFile.write( "\n" )
oStartFile.write( "for ID in ${!SLAVELIST}\n" )
oStartFile.write( "do\n" )
oStartFile.write( "\tif [ $ID -lt 10 ];\n" )
oStartFile.write( "\tthen\n" )
oStartFile.write( "\t\tHOSTNAME=linuxgpus0$ID\n" )
oStartFile.write( "\telse\n" )
oStartFile.write( "\t\tHOSTNAME=linuxgpus$ID\n" )
oStartFile.write( "\tfi\n" )
oStartFile.write( "\n" )
oStartFile.write( "\tlet SLAVENAMEINDEX=$ID-1\n" )
oStartFile.write( "\tlet SLAVENAMEINDEX=$SLAVENAMEINDEX*2\n" )
oStartFile.write( "\tSLAVENAME_FIRST=${SLAVENAMES[$SLAVENAMEINDEX]]}\n" )
oStartFile.write( "\tlet SLAVENAMEINDEX=$SLAVENAMEINDEX+1\n" )
oStartFile.write( "\tSLAVENAME_SECOND=${SLAVENAMES[$SLAVENAMEINDEX]]}\n" )
oStartFile.write( "\n" )
oStartFile.write( "\techo start on $HOSTNAME as ${SLAVENAME_FIRST}\n" )
oStartFile.write( "\tssh $HOSTNAME \"cd ${DIR}; export XAUTHORITY=/var/run/Xauthority-vr;export DISPLAY=:0.0; ./slave_" + sCaveName + ".sh ${SLAVENAME_FIRST}\" &\n" )
oStartFile.write( "\n" )
oStartFile.write( "\techo start on $HOSTNAME as ${SLAVENAME_SECOND}\n" )
oStartFile.write( "\tssh $HOSTNAME \"cd ${DIR}; export XAUTHORITY=/var/run/Xauthority-vr;export DISPLAY=:0.1; ./slave_" + sCaveName + ".sh ${SLAVENAME_SECOND}\" &\n" )
oStartFile.write( "\t\n" )
oStartFile.write( "done\n" )
		
oFile.close()
oStartFile.close()

os.chmod( sStartscriptFilename, stat.S_IRWXU | stat.S_IRWXG ) 
