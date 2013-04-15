import os, sys, shutil, re

cppreplaces = [
( re.compile( r'(^|\W)C(Vista|Vdfn|DLV)', re.MULTILINE ), r'\1\2' ),
( re.compile( r'<VistaAspects\/Vista(BaseTypes|DefaultTimer|ExceptionBase|MathBasics|Quaternion|ReferenceFrame|StreamManager|Streams|StreamUtils|Timer|TimerInterface|TimeUtils|TransformMatrix|Vector3D|VectorMath|Version)\.h\>', re.MULTILINE ), r'<VistaBase/Vista\1.h>' ),
( re.compile( r'\<VistaMath\/VistaVectorMath\.h\>', re.MULTILINE ), r'<VistaBase/VistaVectorMath.h>' ),
( re.compile( r'\<VistaAspects\/Half', re.MULTILINE ), r'<VistaBase/Half' ),
( re.compile( r'\<VistaTools\/VistaTimer\.h\>', re.MULTILINE ), r'<VistaBase/VistaTimer.h>' ),
( re.compile( r'\[(X|Y|Z|W)\]', re.MULTILINE ), r'[Vista::\1]' ),
( re.compile( r'(G|S)etPROPLIST(Value|Ref|ConstRef)', re.MULTILINE ), r'\1etPropertyList\2' ),
( re.compile( r'(G|S)etProp(StringValue|IntValue|DoubleValue|BoolValue|IdValue|StringListValue)', re.MULTILINE ), r'\1et\2' ),
( re.compile( r'(G|S)etProp(Value|Ref|ConstRef)', re.MULTILINE ), r'\1etProperty\2' ),
( re.compile( r'(G|S)etPropPROPLISTValue', re.MULTILINE ), r'\1etPropertyListValue' ),
( re.compile( r'SerializeProplist', re.MULTILINE ), r'SerializePropertyList' ),
( re.compile( r'MergePropLists', re.MULTILINE ), r'MergePropertyLists' ),
( re.compile( r'PrintPROPLIST', re.MULTILINE ), r'PrintPropertyList' ),
( re.compile( r'PROPT_PROPLIST', re.MULTILINE ), r'PROPT_PROPERTYLIST' ),
( re.compile( r'(^|\W)PROPLIST', re.MULTILINE ), r'\1VistaPropertyList' ),
( re.compile( r'(^|\W)VISTA_PI', re.MULTILINE ), r'\1Vista::Pi' ),
( re.compile( r'(^|\W)VISTA_EPSILON', re.MULTILINE ), r'\1Vista::Epsilon' ),
( re.compile( r'\<VistaKernel\/GraphicsManager\/OpenGLDrawInterface\.h\>', re.MULTILINE ), r'<VistaKernel/GraphicsManager/VistaOpenGLDraw.h>' ),
( re.compile( r'\<VistaKernel\/GraphicsManager\/OpenGLDebug\.h\>', re.MULTILINE ), r'<VistaKernel/GraphicsManager/VistaOpenGLDebug.h>' ),
( re.compile( r'(^|\W)OpenGLDebug', re.MULTILINE ), r'\1VistaOpenGLDebug' ),
( re.compile( r'(^|\W)IOpenGLDraw', re.MULTILINE ), r'\1IVistaOpenGLDraw' ),
( re.compile( r'VdfnComposeNode', re.MULTILINE ), r'VdfnMatrixComposeNode' ),
( re.compile( r'(GetWorldTransformation|GetGlobalTransformation)', re.MULTILINE ), r'GetWorldTransform' ),
( re.compile( r'(^|\W)GetWorldTransformation', re.MULTILINE ), r'\1GetParentWorldTransform' ),
( re.compile( r'(^|\W)GetGlobalPosition', re.MULTILINE ), r'\1GetWorldPosition' ),
( re.compile( r'(^|\W)GetRotationAQ', re.MULTILINE ), r'\1GetRotation' ),
( re.compile( r'(^|\W)SetRotationQ', re.MULTILINE ), r'\1SetRotation' ),
( re.compile( r'(^|\W)GetContextFrame', re.MULTILINE ), r'\1GetParentWorldTransform' ),
( re.compile( r'(\.|->)angle', re.MULTILINE ), r'\1m_fAngle' ),
( re.compile( r'(\.|->)axis', re.MULTILINE ), r'\1m_v3Axis' ),
( re.compile( r'(^|\W)C(Vista|Vdfn|DLV)', re.MULTILINE ), r'\1\2' ),
( re.compile( r'(\.|->)GetNorm(\W)', re.MULTILINE ), r'\1GetLengthSquared\2' ),
( re.compile( r'(\.|->)Identity(\W)', re.MULTILINE ), r'\1SetToIdentity\2' ),
#( re.compile( r'(\.|->)GetVal(\W)', re.MULTILINE ), r'\1GetValue\2' ),
( re.compile( r'(VistaDegToRad)', re.MULTILINE ), r'Vista::DegToRad' ),
( re.compile( r'(VistaRadToDeg)', re.MULTILINE ), r'Vista::RadToDeg' ),
]

xmlreplaces = [
( re.compile( r'fromport="val_out"', re.MULTILINE ), r'fromport="out"' ),
( re.compile( r'fromport="val_in"', re.MULTILINE ), r'fromport="in"' ),
( re.compile( r'type="Compose"', re.MULTILINE ), r'type="MatrixCompose"' ),
( re.compile( r'CVista', re.MULTILINE ), r'Vista' ),
]

vcprojreplaces = [
( re.compile( r'VistaAspects(?P<postfix>\$\(NamePostfix\)\.lib|D\.lib|\.lib)', re.MULTILINE ), r'VistaAspects\g<postfix> VistaBase\g<postfix>' ),
( re.compile( r'VistaBase(?P<postfix>\$\(NamePostfix\)\.lib|D\.lib|\.lib)(?P<between>.*) VistaBase(\$\(NamePostfix\)\.lib|D\.lib|\.lib)', re.MULTILINE ), r'VistaBase\g<postfix>\g<between>' ),
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
			if re.search( r'.+\.(cpp|h)$', name ):
				ReplaceContent( fullName, cppreplaces )
			if re.search( r'.+\.(xml)$', name ):
				ReplaceContent( fullName, xmlreplaces )
			if re.search( r'.+\.(vcproj)$', name ):
				ReplaceContent( fullName, vcprojreplaces )
			
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