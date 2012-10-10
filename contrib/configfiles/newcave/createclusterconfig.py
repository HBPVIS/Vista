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

sSlaveIpConfigFileLocation = "/home/vrsw/gpucluster"

aConfigs = [
	( "NoSlaves", [] ),
	( "LeftWall", [2] ),
	( "FrontWall", [3] ),
	( "RightWall", [4] ),
	( "Door", [5] ),
	( "HoloSpace", [0, 1, 2, 3, 4, 5] ),
	( "CaveSpace", [0, 1, 2, 3, 4] ),
	( "Angle", [2, 3] ),
	( "AngleSpace", [0, 1, 2, 3] ),
	( "CaveSpaceNoFloor", [2, 3, 4, 5] ),
	( "HoloSpaceNoFloor", [2, 3, 4] ),
]

nCount = len(aWalls) * len(aRows) * len(aColumns) * len(aEyes)

aSlaveIPsInfiniband = [ "192.168.88.20", "192.168.88.22", "192.168.88.23", "192.168.88.24", "192.168.88.25", "192.168.88.26", "192.168.88.27", "192.168.88.28", "192.168.88.29", "192.168.88.30", "192.168.88.31", "192.168.88.32", "192.168.88.33", "192.168.88.34", "192.168.88.35", "192.168.88.36", "192.168.88.37", "192.168.88.38", "192.168.88.39", "192.168.88.40", "192.168.88.41", "192.168.88.42", "192.168.88.43", "192.168.88.44", "192.168.88.45", "192.168.88.46", "192.168.88.47" ]
aSlaveIPSClusterNet = [ "134.61.201.192", "134.61.201.193", "134.61.201.194", "134.61.201.195", "134.61.201.196", "134.61.201.197", "134.61.201.198", "134.61.201.199", "134.61.201.200", "134.61.201.201", "134.61.201.202", "134.61.201.203", "134.61.201.204", "134.61.201.205", "134.61.201.206", "134.61.201.207", "134.61.201.208", "134.61.201.209", "134.61.201.210", "134.61.201.211", "134.61.201.212", "134.61.201.213", "134.61.201.214", "134.61.201.215", "134.61.201.217", "134.61.201.218", "134.61.201.219" ]


# Create display_newcave.ini

sFilename = "display_" + sCaveName + ".ini"
oFile  = open( sFilename, 'w' )


def WriteVec( aVec ):
	oFile.write( str( aVec[0] ) + ", " + str( aVec[1] ) + ", " + str( aVec[2] ) + "\n" )

def GetIPForSlave( nNumber ):
	nNumber = int(0.5*nNumber+0.5)
	# we have to switch row and column for the floor, i.e. the first 8 slaves
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
	

def GetNameForNode( nWallId, nRowId, nColumnId ):
#	if( nWallId < 2 ):
#		return aWalls[nWallId] + "_" + str( 2*nRowId + nColumnId )
#	else:
		return aWalls[nWallId] + "_" + aRows[nRowId] + aColumns[nColumnId]
		
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
	nNodeID = int( 0.5 * ( nId + 1 ) )
	if nNodeID < 10:
		oFile.write( "SLAVEIP             = ${VISTA_SLAVENODE0" + str(nNodeID) + "_IP}\n" )
	else:
		oFile.write( "SLAVEIP             = ${VISTA_SLAVENODE" + str(nNodeID) + "_IP}\n" )
	oFile.write( "SLAVEPORT           = " + str(nSlavePort) + "\n" )
	oFile.write( "ACKPORT             = " + str(nAckPort) + "       # for old master/slave\n" )
	oFile.write( "SYNC                = TRUE        # for old master/slave\n" )
	oFile.write( "SYNCIP              = " + sBCAddress  +"  # for old master/slave\n" )
	oFile.write( "SYNCPORT            = " + str(nSyncPort) + "       # for old master/slave\n" )
	oFile.write( "DOGLFINISH          = TRUE\n" )
	oFile.write( "BYTESWAP            = FALSE\n" )
	oFile.write( "OUTPUT              = ${OUTSTREAMS_SLAVE}\n" )
	oFile.write( "FREEPORTS           = " + str(nPortRangeStart) + "-" + str(nPortRangeEnd) + "\n" )
	oFile.write( "\n\n" )
	
def CreateMasterSystemSection():
	nConfigCount = 0
	for Config in aConfigs:
		sName = Config[0]
		aWallConfigs = Config[1]
		
		oFile.write( "[" + sName + "]\n" )
		oFile.write( "NAME                = " + sName + "\n" )
		oFile.write( "SLAVES              = " )
		for nWall in aWallConfigs:
			for nRow in range(len(aRows)):
				for nColumn in range(len(aColumns)):
					for nEye in range(len(aEyes)):
						sName = GetNameFor( nWall, nRow, nColumn, nEye )
						oFile.write( sName )
						if nWall != aWallConfigs[len(aWallConfigs)-1] or nRow+1 != len(aRows) or nColumn+1 != len(aColumns) or nEye+1 != len(aEyes):
							oFile.write( ", " )
		oFile.write( "\n" )
		oFile.write( "DISPLAYSYSTEMS      = DISPLAY_MASTER\n" )
		oFile.write( "SYNC                = TRUE            # for old master/slave\n" )
		oFile.write( "SYNCIP              = " + sBCAddress + "      # for old master/slave\n" )
		oFile.write( "SYNCPORTS           = 18990, 18991    # for old master/slave\n" )		
		oFile.write( "BROADCASTIP         = " + sBCAddress + "\n" )
		oFile.write( "BROADCASTPORTS      = 15000-15999\n" )
		oFile.write( "ZEROMQ_ADDRESS      = " + sZeroMQAddress + "\n" )
		oFile.write( "ZEROMQ_PORTS        = " + sZeroMQPorts + "\n" )
		oFile.write( "BARRIERTYPE         = ${BARRIER_MODE}\n" )
		oFile.write( "SWAPSYNCTYPE        = ${SWAPSYNC_MODE}\n" )
		oFile.write( "DATASYNCTYPE        = ${DATASYNC_MODE}\n" )
		oFile.write( "SWAPSYNCTIMEOUT     = 0               # in ms, 0 means infinite wait\n" )
		oFile.write( "OUTPUT              = ${OUTSTREAMS_MASTER}\n" )
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
	oFile.write( "TITLE               = ${WINDOW_TITLE}\n" )
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
	oFile.write( "TITLE               = ${WINDOW_TITLE}\n" )
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
	oFile.write( "MASTER_VSYNC_MODE   = TRUE\n" )
	oFile.write( "SLAVE_VSYNC_MODE    = TRUE\n" )
	oFile.write( "CLIPPING_RANGE      = 0.05, 1000.0\n" )
	oFile.write( "DATASYNC_MODE       = ZEROMQ\n" )
	oFile.write( "BARRIER_MODE        = BROADCAST\n" )
	oFile.write( "SWAPSYNC_MODE       = BROADCAST\n" )	
	oFile.write( "WINDOWINGTK         = GLUT\n" )
	oFile.write( "WINDOW_TITLE        = ViSTA\n" )
	oFile.write( "OUTSTREAMS_MASTER   = COLORCONSOLE_OUTSTREAMS\n" )
	oFile.write( "OUTSTREAMS_SLAVE    = FILE_SPLITERRORS_OUTSTREAMS\n" )
	oFile.write( "\n\n" )
	
def CreateStreamDefintions():
	oFile.write( "########################################\n" )
	oFile.write( "## OUT-STREAM SPECIFICATION\n" )
	oFile.write( "########################################\n" )
	oFile.write( "\n" )
	oFile.write( "# unmodified output to std streams\n" )
	oFile.write( "[DEFAULT_OUTSTREAMS]\n" )
	oFile.write( "THREADSAFE          = FALSE\n" )
	oFile.write( "OUT                 = COUT\n" )
	oFile.write( "WARN                = CERR\n" )
	oFile.write( "ERR                 = CERR\n" )
	oFile.write( "DEBUG               = BUILDTYPE( COUT, NULL )\n" )
	oFile.write( "\n" )
	oFile.write( "# output warnings and errors to color streams\n" )
	oFile.write( "[COLORCONSOLE_OUTSTREAMS]\n" )
	oFile.write( "THREADSAFE          = TRUE\n" )
	oFile.write( "OUT                 = COUT\n" )
	oFile.write( "WARN                = COLOR( BLACK, YELLOW )\n" )
	oFile.write( "ERR                 = COLOR( BLACK, RED )\n" )
	oFile.write( "DEBUG               = BUILDTYPE( OUT, NULL )\n" )
	oFile.write( "\n" )
	oFile.write( "# redirect all output to a file\n" )
	oFile.write( "[FILE_OUTSTREAMS]\n" )
	oFile.write( "THREADSAFE          = TRUE\n" )
	oFile.write( "OUT                 = FILE( slavelogs/log_, ADD_NODENAME )\n" )
	oFile.write( "WARN                = OUT\n" )
	oFile.write( "ERR                 = OUT\n" )
	oFile.write( "DEBUG               = BUILDTYPE( OUT, NULL )\n" )
	oFile.write( "\n" )
	oFile.write( "# redirect all output to a file, but show warnings and errors as color messages\n" )
	oFile.write( "[FILE_SPLITERRORS_OUTSTREAMS]\n" )
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



# creating the slaveip_config
sSlaveConfigFilename = "slavenodes_ipconfig_" + sCaveName + ".sh"
oNodesConfig  = open( sSlaveConfigFilename, 'w' )

oNodesConfig.write( "#!/bin/bash\n" )
oNodesConfig.write( "# This script defines the slaves for the aixCAVE cluster\n" )
oNodesConfig.write( "# It defines all slaves (or, technically, the nodes for two slaves each) and exports the required variables\n" )
oNodesConfig.write( "# slaves are defined as arrays of the form ( name_prefix hostname(fors ssh) ip(for vista) [ [DISABLED|WARNING|IP_CHANGED] Message ]\n" )
oNodesConfig.write( "\n" )
oNodesConfig.write( "export SLAVENODES_COUNT=" + str( len(aWalls) * len(aRows) * len(aColumns) ) + "\n" )
nCount = 0
for nWall in range(len(aWalls)):
	for nRow in range(len(aRows)):
		for nColumn in range(len(aColumns)):
			nCount = nCount + 1
			sName = GetNameForNode( nWall, nRow, nColumn )
			if nCount < 10:
				sExpCount = "0" + str(nCount)
			else:
				sExpCount = str(nCount)
				
			# swap floors
			if nCount == 2:
				nHostCount = 3
			elif nCount == 3:
				nHostCount = 2
			elif nCount == 6:
				nHostCount = 7
			elif nCount == 7:
				nHostCount = 6
			else:
				nHostCount = nCount
				
			if nCount < 10:
				sHost = "linuxgpus0" + str(nHostCount)
			else:
				sHost = "linuxgpus" + str(nHostCount)
			sIP = GetIPForSlave( 2*nCount )
			oNodesConfig.write( "export SLAVENODE" + sExpCount + "=( " + sName.ljust( 16 ) + " " + sHost.ljust(12) + " " + sIP.ljust(12) + " )\n" )
oNodesConfig.write( "\n" )
oNodesConfig.write( "# export slave variables\n" )
oNodesConfig.write( "for (( i=1; i<=${SLAVENODES_COUNT}; i++ ));\n" )
oNodesConfig.write( "do\n" )
oNodesConfig.write( "    if (( i<10 )); then\n" )
oNodesConfig.write( "        SLAVENODEIPNAME=VISTA_SLAVENODE0${i}_IP\n" )
oNodesConfig.write( "        SLAVENODE=SLAVENODE0${i}\n" )
oNodesConfig.write( "    else\n" )
oNodesConfig.write( "        SLAVENODEIPNAME=VISTA_SLAVENODE${i}_IP\n" )
oNodesConfig.write( "        SLAVENODE=SLAVENODE${i}\n" )
oNodesConfig.write( "    fi\n" )
oNodesConfig.write( "\n" )	
oNodesConfig.write( "    eval export ${SLAVENODEIPNAME}=\${$SLAVENODE[2]}\n" )
oNodesConfig.write( "done\n" )
oNodesConfig.write( "\n" )




# creating the start script
sStartscriptFilename = "startslaves_" + sCaveName + ".sh"
oStartFile  = open( sStartscriptFilename, 'w' )

oStartFile.write( "#!/bin/bash\n\n" )
oStartFile.write( "\n" )
oStartFile.write( "# generic script to start the slaves of the new CAVE at the RWTH Aachen University\n" )
oStartFile.write( "\n" )
oStartFile.write( "# define the colors for node status messages\n" )
oStartFile.write( "DISABLED_COLOR=$(echo -e \"\\033[0;30;41m\") # black on red\n" )
oStartFile.write( "SKIPPED_COLOR=$(echo -e \"\\033[0;30;41m\") # black on red\n" )
oStartFile.write( "WARN_COLOR=$(echo -e \"\\033[0;30;43m\") # black on yellow/brown\n" )
oStartFile.write( "IPCHANGED_COLOR=$(echo -e \"\\033[1;32m\") # green\n" )
oStartFile.write( "RESET_COLOR=$(echo -e \"\\033[0m\")\n" )
oStartFile.write( "\n" )
oStartFile.write( "CONFIGS=( " )
for Config in aConfigs:
	oStartFile.write( str( Config[0] ) + " " )
oStartFile.write( " )\n" )
for Config in aConfigs:
	oStartFile.write( Config[0] + "=( " )
	for nScreenNum in Config[1]:
		oStartFile.write( str( 4*nScreenNum + 1) + " " + str( 4*nScreenNum + 2 ) + " " + str( 4*nScreenNum + 3 ) + " " + str( 4*nScreenNum + 4 ) + " " )
	oStartFile.write( ")\n" )
oStartFile.write( "\n" )
oStartFile.write( "#########################################\n" )
oStartFile.write( "# Generic, application-independent file #\n" )
oStartFile.write( "# Don't edit!                           #\n" )
oStartFile.write( "# (unless you know what you're doing)   #\n" )
oStartFile.write( "#########################################\n" )
oStartFile.write( "\n" )
oStartFile.write( "# load slave ip config settings to know each slave's current IP\n" )
oStartFile.write( "if ! [ -f \"$SLAVENODES_CONFIGURATION_FILE\" ]; then\n" )
oStartFile.write( "    echo \"no SLAVENODES_CONFIGURATION_FILE set - resetting to default\"\n" )
oStartFile.write( "    $SLAVENODES_CONFIGURATION_FILE=\"/home/vrsw/gpucluster/slavenodes_ipconfig_newcave.sh\"\n" )
oStartFile.write( "fi\n" )
oStartFile.write( "source $SLAVENODES_CONFIGURATION_FILE\n" )
oStartFile.write( "\n" )
oStartFile.write( "# determine configuration\n" )
oStartFile.write( "if [ \"$1\" = \"\" ];\n" )
oStartFile.write( "then\n" )
oStartFile.write( "    echo 'parameter missing - specify a cluster configuration ( ${CONFIGS[@]} )'\n" )
oStartFile.write( "    exit\n" )
oStartFile.write( "fi\n" )
oStartFile.write( "\n" )
oStartFile.write( "INPUTCONFIG=${1,,}\n" )
oStartFile.write( "CHOSENCONFIG=\"\"\n" )
oStartFile.write( "\n" )
oStartFile.write( "for TESTCONFIG in ${CONFIGS[@]}\n" )
oStartFile.write( "do    \n" )
oStartFile.write( "    if [ \"${TESTCONFIG,,}\" == \"$INPUTCONFIG\" ];\n" )
oStartFile.write( "    then\n" )
oStartFile.write( "        CHOSENCONFIG=$TESTCONFIG\n" )
oStartFile.write( "        shift\n" )
oStartFile.write( "        break\n" )
oStartFile.write( "    fi\n" )
oStartFile.write( "done\n" )
oStartFile.write( "\n" )
oStartFile.write( "if [ \"$CHOSENCONFIG\" == \"\" ];\n" )
oStartFile.write( "then\n" )
oStartFile.write( "    echo \"Provided Config does not exist\"\n" )
oStartFile.write( "    echo \"Configs are: ${CONFIGS[@]}\"\n" )
oStartFile.write( "    exit\n" )
oStartFile.write( "else\n" )
oStartFile.write( "    echo \"Starting config $CHOSENCONFIG\"\n" )
oStartFile.write( "fi\n" )
oStartFile.write( "\n" )
oStartFile.write( "SLAVELIST=\"$CHOSENCONFIG[@]\"\n" )
oStartFile.write( "\n" )
oStartFile.write( "# determine if slave output should be redirected\n" )
oStartFile.write( "if $REDIRECT_SLAVE_OUTPUT; then\n" )
oStartFile.write( "    \n" )
oStartFile.write( "    echo \"Redirecting all slaves' output to ./slavelogs/slave_*.log\"\n" )
oStartFile.write( "    if [ ! -d \"slavelogs\" ]; then\n" )
oStartFile.write( "        mkdir slavelogs\n" )
oStartFile.write( "    fi\n" )
oStartFile.write( "    \n" )
oStartFile.write( "fi\n" )
oStartFile.write( "\n" )
oStartFile.write( "# set prefix for calls (always the same)\n" )
oStartFile.write( "DIR=`pwd`\n" )
oStartFile.write( "SSH_CALL_PREFIX=\"cd ${DIR}; export XAUTHORITY=/var/run/Xauthority-vr; export DISPLAY=:0.0; export SLAVENODES_CONFIGURATION_FILE=${SLAVENODES_CONFIGURATION_FILE}\"\n" )
oStartFile.write( "\n" )
oStartFile.write( "for ID in ${!SLAVELIST}\n" )
oStartFile.write( "do\n" )
oStartFile.write( "    \n" )
oStartFile.write( "    let SLAVENODEINDEX=$ID-1\n" )
oStartFile.write( "    if [ $ID -lt 10 ];\n" )
oStartFile.write( "    then\n" )
oStartFile.write( "        SLAVENODESECTION=SLAVENODE0${ID}\n" )
oStartFile.write( "        SLAVESKIPVAR=SKIP_SLAVE0${ID}\n" )
oStartFile.write( "    else\n" )
oStartFile.write( "        SLAVENODESECTION=SLAVENODE${ID}\n" )
oStartFile.write( "        SLAVESKIPVAR=SKIP_SLAVE${ID}\n" )
oStartFile.write( "    fi\n" )
oStartFile.write( "    \n" )
oStartFile.write( "    eval SLAVENODENAME=\${$SLAVENODESECTION[0]}\n" )
oStartFile.write( "    eval SLAVEHOST=\${$SLAVENODESECTION[1]}\n" )
oStartFile.write( "    eval SLAVESTATUS=\${$SLAVENODESECTION[3]}\n" )
oStartFile.write( "    eval SLAVEMESSAGE=\${$SLAVENODESECTION[4]}\n" )
oStartFile.write( "        \n" )
oStartFile.write( "    if [ \"${SLAVESTATUS}\" == \"DISABLED\" ]; then\n" )
oStartFile.write( "        echo \"${DISABLED_COLOR}##########################################\"\n" )
oStartFile.write( "        echo \"# Slave ${ID} currently disabled \"\n" )
oStartFile.write( "        echo \"# ${SLAVEMESSAGE}\"\n" )
oStartFile.write( "        echo \"##########################################${RESET_COLOR}\"\n" )
oStartFile.write( "        continue\n" )
oStartFile.write( "    elif [ \"${SLAVESTATUS}\" == \"WARNING\" ]; then\n" )
oStartFile.write( "        echo \"${WARN_COLOR}###########################################\"\n" )
oStartFile.write( "        echo \"# Slave ${ID} WARNING:  ${SLAVEMESSAGE}\"\n" )
oStartFile.write( "        echo \"##########################################${RESET_COLOR}\"\n" )
oStartFile.write( "    elif [ \"${SLAVESTATUS}\" == \"IP_CHANGED\" ]; then\n" )
oStartFile.write( "        echo \"${IPCHANGED_COLOR}Slave ${ID} has changed IP:  ${SLAVEMESSAGE}${RESET_COLOR}\"\n" )
oStartFile.write( "    fi\n" )
oStartFile.write( "    \n" )
oStartFile.write( "    if [ \"${!SLAVESKIPVAR}\" == \"1\" ]; then\n" )
oStartFile.write( "        echo \"${SKIPPED_COLOR}Slave ${ID} skipped by command line param ${RESET_COLOR}\"\n" )
oStartFile.write( "        continue\n" )
oStartFile.write( "    fi\n" )
oStartFile.write( "\n" )
oStartFile.write( "    SLAVENAME_FIRST=${SLAVENODENAME}_RE\n" )
oStartFile.write( "    SLAVENAME_SECOND=${SLAVENODENAME}_LE    \n" )
oStartFile.write( "\n" )
oStartFile.write( "    if $REDIRECT_SLAVE_OUTPUT; then\n" )
oStartFile.write( "\n" )
oStartFile.write( "        echo \"starting slave ${SLAVENAME_FIRST} on ${SLAVEHOST}\"\n" )
oStartFile.write( "        ssh $SLAVEHOST \"${SSH_CALL_PREFIX}; ./startscripts/slave_newcave.sh ${SLAVENAME_FIRST} $@ >slavelogs/slave_${SLAVENAME_FIRST}.log 2>&1\" &\n" )
oStartFile.write( "\n" )
oStartFile.write( "        echo \"starting slave ${SLAVENAME_SECOND} on ${SLAVEHOST}\"\n" )
oStartFile.write( "        ssh $SLAVEHOST \"${SSH_CALL_PREFIX}; ./startscripts/slave_newcave.sh ${SLAVENAME_SECOND} $@ >slavelogs/slave_${SLAVENAME_SECOND}.log 2>&1\" &\n" )
oStartFile.write( "    \n" )
oStartFile.write( "    else\n" )
oStartFile.write( "\n" )
oStartFile.write( "        echo \"starting slave ${SLAVENAME_FIRST} on ${SLAVEHOST}\"\n" )
oStartFile.write( "        ssh $SLAVEHOST \"${SSH_CALL_PREFIX}; ./startscripts/slave_newcave.sh ${SLAVENAME_FIRST} $@\" &\n" )
oStartFile.write( "\n" )
oStartFile.write( "        echo \"starting slave ${SLAVENAME_SECOND} on ${SLAVEHOST}\"\n" )
oStartFile.write( "        ssh $SLAVEHOST \"${SSH_CALL_PREFIX}; ./startscripts/slave_newcave.sh ${SLAVENAME_SECOND} $@\" &\n" )
oStartFile.write( "\n" )
oStartFile.write( "    fi\n" )
oStartFile.write( "done\n" )
		
oFile.close()
oStartFile.close()

os.chmod( sStartscriptFilename, stat.S_IRWXU | stat.S_IRWXG ) 
