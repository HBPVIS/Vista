import os, sys, shutil, re, datetime

CurrentYear = str( datetime.datetime.now().year )
BackupPostfix = "BEFORE_COPYRIGHTYEAR_UPDATE"

cppreplaces = [
	( re.compile( r'([ ]*/[\*]*[ ]*)Copyright \(c\)([ ]+[0-9]{4})\-[0-9]{4}', re.MULTILINE ), r'\1Copyright (c)\2-' + CurrentYear ),	
]
licensereplaces = [
	( re.compile( r'Copyright \(c\)([ ]+[0-9]{4})\-[0-9]{4}', re.MULTILINE ), r'Copyright (c)\1-' + CurrentYear ),	
]
texreplaces = [
	( re.compile( r'Copyright \(c\)([ ]+[0-9]{4})\-[0-9]{4}', re.MULTILINE ), r'Copyright (c)\1-' + CurrentYear ),	
]

replacementrules = [
	( r'.+\.(cpp|h)$', cppreplaces ),
	( r'.+\.(cpp|h).txt$', cppreplaces ),
	( r'.+\.tex$', texreplaces ),
	( r'LICENSE.*', licensereplaces ),
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
		backupFile = file + '.' + BackupPostfix + ".BAK"
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
		match = re.search( r'(.+)\.' + BackupPostfix + '\.BAK$', name )
		if match:
			fullName = os.path.join( root, name )
			fullRestoreName = os.path.join( root, match.group(1) )
			print( "reverting file " + fullRestoreName )
			os.remove( fullRestoreName )
			os.rename( fullName, fullRestoreName )


if len(sys.argv) > 1 and sys.argv[1] != "":
	if len(sys.argv) > 2 and sys.argv[2] == "-undo":
		startdir = sys.argv[1]	
		print( 'undo startdir: ' + startdir )
		for root, dirs, files in os.walk( startdir ):
			UndoBackup( root, files )
	else:
		startdir = str( sys.argv[1] )
		print( 'startdir: [' + startdir + ']' )
		for root, dirs, files in os.walk( startdir ):
			ProcessFiles( root, files )
else:
	print( 'Usage:' )
	print( sys.argv[0] + ' ConversionRootDir [-undo]' )
	print( '  ConversionRootDir specifies toplevel dir from which on all files and subfolders will be converted, automatically backing up changed files' )
	print( '  -undo - If provided, the backups from a prior conversion will be restored. NOTE: This removes all changes from th econversion, but also all amnual changes after the backup!' )