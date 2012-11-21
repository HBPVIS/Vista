import os, sys, math

bUseTabs = False
nTabSize = 4
nMaxPrefixLength = 24
bRightAlign = False
bIgnoreCommentSign = True

def FindPrefixLength( aFileContent ):
	nMaxLength = 0
	for sLine in aFileContent:
		nPos = sLine.find( "=" )
		if nPos != -1:
			sPrefix = sLine[:nPos].strip()
			if bIgnoreCommentSign and sPrefix[0] == "#":
				sPrefix = sPrefix[1:]
			if len(sPrefix) > nMaxLength:
				nMaxLength = len(sPrefix)
	if nMaxLength > nMaxPrefixLength:
		nMaxLength = nMaxPrefixLength
	return nMaxLength
	
def ReformatLine( sLine, nPrefixLength ):
	nPos = sLine.find( "=" )
	if nPos == -1:
		return sLine
	nKey = sLine[:nPos].strip()
	nValue = sLine[nPos+1:].strip()
	
	if bUseTabs:
		if bRightAlign:
			print( "Warning: RightAlign incompatible with UseTabs - using LeftAlign" )
		
		# we need one more - before the equal sign
		nTargetTabs = int( math.ceil( ( float(nPrefixLength) + 1.0 ) / float(nTabSize) ) )
		if bIgnoreCommentSign and nKey[0] == "#":
			nWordTabs = int( float(len(nKey)) / float(nTabSize) )
		else:
			nWordTabs = int( float(len(nKey)-1) / float(nTabSize) )
		nMissingTabs = nTargetTabs - nWordTabs
		
		if nAddSpaces <= 0:
			nReturn = nKey + "\t"
		else:			
			nReturn = nKey;
			for i in range( nMissingTabs ):
				nReturn = nReturn + "\t"	
		
	else:
		nAddSpaces = nPrefixLength - len(nKey)
		bTargetLength = nPrefixLength
		if bIgnoreCommentSign and nKey[0] == "#":
			nAddSpaces = nAddSpaces + 1
			bTargetLength = nPrefixLength + 1
		if nAddSpaces <= 0:
			nReturn = nKey + " "
		elif bRightAlign:
			nReturn = nKey.rjust( bTargetLength ) + " "
		else:
			nReturn = nKey.ljust( bTargetLength ) + " "
	
		
	nReturn = nReturn + "= " + nValue + "\n"
	return nReturn
	
	
def ProcessFile( sFilename ):
	try:
		oFile = open( sFilename, "r" )
		aFileContent = oFile.readlines()
	except(IOError), e:
		print( "Unable to read file \"" + sFilename + "\"\n" )
		return		
	
	oFile.close()
		
	nPrefixLength = FindPrefixLength( aFileContent )
	print( "max: " + str(nPrefixLength) )
	
	aContent = ""
	for sLine in aFileContent:
		aContent = aContent + ReformatLine( sLine, nPrefixLength )
	
	try:
		oFile = open( sFilename, "w" )
		oFile.write( aContent )
	except(IOError), e:
		print( "Unable to write file \"", sFilename, "\"\n" )
	return	
			


if len(sys.argv) > 1 and sys.argv[1] != "":
	ProcessFile( sys.argv[1] )
else:
	print( "Usage: " + sys.argv[0] + " file_to_reformat" )

