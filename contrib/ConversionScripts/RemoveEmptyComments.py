import os, sys, shutil, re

#MSVC ^:b*/\*\*:b*(\n:b*\*:b*)*(\n)*:b*\*/:b*\n

cppreplaces = [
( re.compile( r'IVistaSimpleTextOverlay', re.MULTILINE ), r'VistaSimpleTextOverlay' ),
( re.compile( r'IVista3DTextOverlay', re.MULTILINE ), r'Vista3DTextOverlay' ),
( re.compile( r'(\.|->)PrintPropertyList', re.MULTILINE ), r'\1Print' ),
]

inireplaces = [
]


vcprojreplaces = [
]


def ReplaceContent( file, replacelist ):    
	#print( '    processing file ' + file )
	input = open( file, 'r' )
	filecontent = input.read()
	input.close()

	replaceCount = 0
	for replacerule in replacelist:
		replacement, rchanges = re.subn( replacerule[0], replacerule[1], filecontent )
		if rchanges > 0:
			filecontent = replacement
			replaceCount = replaceCount + rchanges

	if replaceCount > 0:
		print ( ' performed', replaceCount, 'replacements in ' + file )
		backupFile = file + '.BEFORE_V1_11_UPDATE.BAK'
		if os.path.isfile( backupFile ) == False:
			print ( '        backing up original file to ' + backupFile )
			shutil.copy2( file, backupFile )
		outFile = open( file, 'r+' )
		outFile.read() 
		outFile.seek( 0 )
		outFile.write( filecontent )
		outFile.truncate()
		outFile.close()
	#else:
	#	print( '         no replacement' )

	  
def ProcessFiles( root, files ):
	for name in files:
		fullName = os.path.join( root, name )
		if os.path.isfile( fullName ):
			if re.search( r'.+\.(cpp|h)$', name ):
				ReplaceContent( fullName, cppreplaces )
			if re.search( r'.+\.(xml)$', name ):
				ReplaceContent( fullName, xmlreplaces )
			if re.search( r'.+\.(vcproj)$', name ):
				ReplaceContent( fullName, vcprojreplaces )
			
def UndoBackup( root, files ):
	for name in files:
		match = re.search( r'(.+)\.BEFORE_V1_11_UPDATE\.BAK$', name )
		if match:
			fullName = os.path.join( root, name )
			fullRestoreName = os.path.join( root, match.group(1) )
			print( "reverting file " + fullRestoreName )
			os.remove( fullRestoreName )
			os.rename( fullName, fullRestoreName )


if len(sys.argv) > 1 and sys.argv[1] != "":
	if len(sys.argv) > 2 and sys.argv[2] == "-undo":
		startdir = sys.argv[1]	
		print( 'startdir: ' + startdir )
		walkres = os.walk( startdir )
		for root, dirs, files in os.walk( startdir ):
			UndoBackup( root, files )
	else:
		startdir = sys.argv[1]
		print( 'startdir: ' + startdir )
		walkres = os.walk( startdir )
		for root, dirs, files in os.walk( startdir ):
			ProcessFiles( root, files )
else:
	print( 'Usage:' )
	print( 'VistaTo1.11.py ConversionRootDir [-undo]' )
	print( '  ConversionRootDir specifies toplevel dir from which on all files and subfolders will be converted, automatically backing up changed files' )
	print( '  -undo - If provided, the backups from a prior conversion will be restored. NOTE: This removes all changes from th econversion, but also all amnual changes after the backup!' )