import os, sys, shutil, re

BackupName ="UPDATE_TO_1.14"

cppreplaces = [	
	( re.compile( r'VistaRandomNumberGenerator::GetSingleton', re.MULTILINE ), r'VistaRandomNumberGenerator::GetStandardRNG' ),
]

inireplaces = [	
]

xmlreplaces = [
]



replacementrules = [
	( r'.+\.(cpp|h)$', cppreplaces ),
	#( r'.+\.(ini)$', inireplaces ),
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
		backupFile = file + '.' + BackupName + '.BAK'
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
		match = re.search( r'(.+)\.' + BackupName + '\.BAK$', name )
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
	print( 'VistaToTrunk.py ConversionRootDir [-undo]' )
	print( '  ConversionRootDir specifies toplevel dir from which on all files and subfolders will be converted, automatically backing up changed files' )
	print( '  -undo - If provided, the backups from a prior conversion will be restored. NOTE: This removes all changes from the conversion, but also all manual changes after the backup!' )