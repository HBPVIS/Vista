import os, sys, shutil, re

cppreplaces = [
	( re.compile( r'IVistaSimpleTextOverlay', re.MULTILINE ), r'VistaSimpleTextOverlay' ),
	( re.compile( r'IVista3DTextOverlay', re.MULTILINE ), r'Vista3DTextOverlay' ),
	( re.compile( r'(\.|->)PrintPropertyList(\s*\()', re.MULTILINE ), r'\1Print\2' ),
	( re.compile( r'(\.|->)GetHasProperty(\s*\()', re.MULTILINE ), r'\1HasProperty\2' ),
	( re.compile( r'(\.|->)GetHasSubList(\s*\()', re.MULTILINE ), r'\1HasSubList\2' ),
	( re.compile( r'VistaColorRGB', re.MULTILINE ), r'VistaColor' ),
	
	( re.compile( r'VistaSG', re.MULTILINE ), r'VistaSceneGraph' ),
	( re.compile( r'(\.|->)(Get|Set)CullingEnabled', re.MULTILINE ), r'\1\2OcclusionCullingEnabled' ),
	( re.compile( r'(\.|->)Add2D(Text|Bitmap|Line|Rectangle)', re.MULTILINE ), r'\1New2D\2' ),
	( re.compile( r'VistaType::ubyte8', re.MULTILINE ), r'VistaType::byte' ),
	
	( re.compile( r'<VistaBase/VistaStreams.h>', re.MULTILINE ), r'<VistaTools/VistaStreams.h>' ),
	( re.compile( r'vstr::CC_([A-Z_]+[^A-Z_])', re.MULTILINE ), r'VistaColorOutstream::CC_\1' ),
	( re.compile( r'vstr::CONSOLE_COLOR', re.MULTILINE ), r'VistaColorOutstream::CONSOLE_COLOR' ),
	( re.compile( r'vstr::GetConsoleColorName', re.MULTILINE ), r'VistaColorOutstream::GetConsoleColorName' ),
	
	( re.compile( r'VistaGeometry::faceType', re.MULTILINE ), r'VistaGeometry::FaceType' ),
	
	( re.compile( r'VistaGetArrayPropertyFunctor', re.MULTILINE ), r'TVistaPropertyArrayGet' ),
	( re.compile( r'VistaSetArrayPropertyFunctor', re.MULTILINE ), r'TVistaPropertyArraySet' ),
	( re.compile( r'VistaPropertyListGetFunctor', re.MULTILINE ), r'TVistaPropertyListGet' ),
	( re.compile( r'VistaPropertyListSetFunctor', re.MULTILINE ), r'TVistaPropertyListSet' ),

# optional - better don't use
# they should be safe, but are better replaced using exact matches
# (e.g. SetValue<float> instead of double where applicable
# propertylist-getters are explicitely not modeled! Check them manually
	#( re.compile( r'(\.|->)SetStringValue(\s*\()', re.MULTILINE ), r'\1SetValue<std::string>\2' ),
	#( re.compile( r'(\.|->)SetIntValue(\s*\()', re.MULTILINE ), r'\1SetValue<int>\2' ),
	#( re.compile( r'(\.|->)SetBoolValue(\s*\()', re.MULTILINE ), r'\1SetValue<bool>\2' ),
	#( re.compile( r'(\.|->)SetDoubleValue(\s*\()', re.MULTILINE ), r'\1SetValue<double>\2' ),
	#( re.compile( r'(\.|->)SetStringListValue(\s*\()', re.MULTILINE ), r'\1SetValue\2' ),
	
# Optional: replace propertylist-getvalues with API-equivalent. Can, but should not be used
# since there are now better options, and for more different types
	#( re.compile( r'(\.|->)GetStringValue(\s*\()', re.MULTILINE ), r'\1GetValueOrDefault<std::string>\2' ),
	#( re.compile( r'(\.|->)GetIntValue(\s*\()', re.MULTILINE ), r'\1GetValueOrDefault<int>\2' ),
	#( re.compile( r'(\.|->)GetBoolValue(\s*\()', re.MULTILINE ), r'\1GetValueOrDefault<bool>\2' ),
	#( re.compile( r'(\.|->)GetDoubleValue(\s*\()', re.MULTILINE ), r'\1GetValueOrDefault<double>\2' ),
	#( re.compile( r'(\.|->)GetStringListValue(\s*\()', re.MULTILINE ), r'\1SetValue\2' ),
	
# this line can be used, but shouldn't - HasSubList now throws an exception, instead of 
# returning an empty proplistref
	#( re.compile( r'(\.|->)GetPropertyListValue(\s*\()', re.MULTILINE ), r'\1HasSubList\2' ),
]

inireplaces = [
	( re.compile( r'DUMPGRAPHS', re.MULTILINE ), r'DUMPDFNGRAPHS' ),
	( re.compile( r'WRITEPORTS', re.MULTILINE ), r'WRITEDFNPORTS' ),
	( re.compile( r'FULL_SCREEN', re.MULTILINE ), r'FULLSCREEN' ),
]

xmlreplaces = [
	( re.compile( r'(\s*param\s*name\s*=\s*)"sensor_id"', re.MULTILINE ), r'\1"sensor_name"' ),
	( re.compile( r'(\s*param\s*name\s*=\s*)"sensor"', re.MULTILINE ), r'\1"sensor_index"' ),
]



replacementrules = [
	( r'.+\.(cpp|h)$', cppreplaces ),
	( r'.+\.(ini)$', inireplaces ),
	( r'.+\.(xml)$', xmlreplaces ),
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
		backupFile = file + '.BEFORE_CLUSTERMODE_UPDATE.BAK'
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
		match = re.search( r'(.+)\.BEFORE_CLUSTERMODE_UPDATE\.BAK$', name )
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
