import os, sys, shutil, re

cppreplaces = [
	( re.compile( r'(\s|\<|\(|\*)(microtime|systemtime|microstamp|sint32|uint32|ushort16|sshort16|float32|ubyte8|byte8|sint64|uint64|float64|val32|val64)(\W)', re.MULTILINE ), r'\1VistaType::\2\3' ),
	( re.compile( r'<VistaDeviceDriversBase/Vista([a-zA-Z]*)Aspect.h>', re.MULTILINE ), r'<VistaDeviceDriversBase/DriverAspects/Vista\1Aspect.h>' ),
]

xmlreplaces = [
]

inireplaces = [
	( re.compile( r'(TYPE\s*=\s*)(MOUSE\s|KEYBOARD\s)', re.MULTILINE ), r'\1GLUT\2' ),
	( re.compile( r'^(\s*)DRIVERPLUGINS(\s*=\s*)\S*(\s*)$', re.MULTILINE ), r'\1DRIVERPLUGINDIRS\2${VISTACORELIBS_DRIVER_PLUGIN_DIRS}\3' ),
	( re.compile( r'^\s*LOADPLUGINS[^\n\r]*[\n\r$]', re.MULTILINE ), r'' ),
]

replacementrules = [
	( r'.+\.(cpp|h)$', cppreplaces ),
	( r'.+\.(ini)$', inireplaces ),
	#( r'.+\.(xml)$', xmlreplaces ),
]


def ReplaceContent( file, replacelist ):    
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
		backupFile = file + '.BEFORE_V110_UPDATE.BAK'
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
			for tuple in replacementrules:
				if re.search( tuple[0], name ):
					ReplaceContent( fullName, tuple[1] )
			
def UndoBackup( root, files ):
	for name in files:
		match = re.search( r'(.+)\.BEFORE_V110_UPDATE\.BAK$', name )
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
	print( 'VistaTo1.10.py ConversionRootDir [-undo]' )
	print( '  ConversionRootDir specifies toplevel dir from which on all files and subfolders will be converted, automatically backing up changed files' )
	print( '  -undo - If provided, the backups from a prior conversion will be restored. NOTE: This removes all changes from the conversion, but also all manual changes after the backup!' )