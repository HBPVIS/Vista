/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2010 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
/*
 * $Id: StackWalkerGeomDemo.cpp$
 */

/*
Description:

This is the StackWalkerGeomDemo. It will show you how to use the VistaStackWalker. Additionally it'll
show you how to use the StackWalker to visualize the CallStack using Text3D.

*/

#include "DebuggingToolsDemo.h"

#include <VistaBase/VistaStreams.h>
#include <VistaBase/VistaStreamManager.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaExceptionBase.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/Stuff/VistaStreamManagerExt.h>
#include <VistaKernel/VistaKernelOut.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaTools/VistaRandomNumberGenerator.h>
#include <VistaTools/VistaStackWalker.h>
#include <VistaTools/VistaBasicProfiler.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace
{
	void RandomFunctionOne();
	void RandomFunctionTwo();
	void RandomFunctionThree();

	// some functions to generate random callstacks
	void RandomFunctionOne()
	{
		switch( VistaRandomNumberGenerator::GetSingleton()->GenerateInt32( 0, 3 ) )
		{
			case 0:
				VistaStackWalker::PrintCallStack( vstr::Stream() );
				break;
			case 1:
				RandomFunctionOne();
				break;
			case 2:
				RandomFunctionTwo();
				break;
			case 3:
				RandomFunctionThree();
				break;
			default:
				break;
		}
	}

	void RandomFunctionTwo()
	{
		switch( VistaRandomNumberGenerator::GetSingleton()->GenerateInt32( 0, 3 ) )
		{
			case 0:
				VistaStackWalker::PrintCallStack( vstr::Stream() );
				break;
			case 1:
				RandomFunctionOne();
				break;
			case 2:
				RandomFunctionTwo();
				break;
			case 3:
				RandomFunctionThree();
				break;
			default:
				break;
		}
	}

	void RandomFunctionThree()
	{
		switch( VistaRandomNumberGenerator::GetSingleton()->GenerateInt32( 0, 3 ) )
		{
			case 0:
				VistaStackWalker::PrintCallStack( vstr::Stream() );
				break;
			case 1:
				RandomFunctionOne();
				break;
			case 2:
				RandomFunctionTwo();
				break;
			case 3:
				RandomFunctionThree();
				break;
			default:
				break;
		}
	}
}

class DebuggingToolsDemoAppl::KeyCallback : public IVistaExplicitCallbackInterface
{
public:
	KeyCallback() {}
	~KeyCallback() {}

	virtual bool Do()
	{
		RandomFunctionOne();
		return true;
	}
};
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


DebuggingToolsDemoAppl::DebuggingToolsDemoAppl( int argc, char  *argv[] )
: VistaEventHandler()
, m_pVistaSystem( new VistaSystem )
, m_pStackWalkerCallback( new KeyCallback )
{
	std::list<std::string> liSearchPaths;

	// here is a shared configfile used for your application use:
	liSearchPaths.push_back("../configfiles/");
	m_pVistaSystem->SetIniSearchPaths(liSearchPaths);

	// During an application run, there is plenty of output onto the console.
	// Especially during initialization, it is easy to miss error messages etc.
	// Thus, it is useful to color the outputs differently. For this, Vista provides
	// the VistaColorOutstream

	// If errors occur, we want them to print yellow on red
	// we also want to prevent buffering, so that all text is output immediately, thus 
	// buffersize is set to 1
	// NOTE: each color stream has his own buffer! Thus, if you combine multiple color
	// streams, or use cout/cerr inbetween, the output may be fragmented! To avoid this, either
	// disable the color streams buffering, or use std::endl or flush the stream manually
	// when finished writing to it. This should become good practice anyway!
	VistaColorOutstream* pRedStream = new VistaColorOutstream( vstr::CC_YELLOW, vstr::CC_RED, 1 );
	//now, we can tell ViSTA to print all internal errors to this stream
	VistaKernelOut::SetAllErrorStreams( pRedStream );

	// To check if this worked, we will now call run on the VistaSystem
	// This will fail since we are not yet initialized, and will output a warning
	// to the specified stream
	m_pVistaSystem->Run();

	// ViSTA's standard intro message
	m_pVistaSystem->IntroMsg ();

	if( !m_pVistaSystem->Init( argc, argv ) )
	{
		(*pRedStream) << "Initialization FAILED - aborting!" << std::endl;
		return;
	}
	if ( m_pVistaSystem->GetDisplayManager()->GetDisplaySystem() == 0 )
		VISTA_THROW( "No DisplaySystem found" , 1 );
	m_pVistaSystem->GetDisplayManager()->GetWindowByName( "MAIN_WINDOW" )->GetWindowProperties()->SetTitle( argv[0] );

	// We register to receive system events
	m_pVistaSystem->GetEventManager()->AddEventHandler( this, 
														VistaSystemEvent::GetTypeId(),
														VistaEventManager::NVET_ALL );

	// In some occasions, especially if an exception occurs or if anything else happens, the
	// current stacktrace may be of interest, showing the current functions to the source path
	// the VistaStackWalker is called from. To show this, we register a key callback that
	// generates a random sequence of function calls, and then prints the stack to out
	// default stream, i.e. the split stream
	// when pressing 's', a quite random stacktrace shall be printed
	m_pVistaSystem->GetKeyboardSystemControl()->BindAction( 's', m_pStackWalkerCallback, "Print StackTrace" );

	// For our own output, we first create a new colorstream
	VistaColorOutstream* pGreenStream = new VistaColorOutstream( vstr::CC_GREEN, vstr::CC_DEFAULT );
	// we now register the red and green streams with the StreamManager. It gains access to all streams
	// registered with it, independent of their type. Since it is a singleton, we can access it from
	// anywhere - this gives us the option to write to a common output, without having to know the actual
	// stream everywhere.
	// Since we are lazy, we also allow the StreamManager to handle deletion of the stream, so we dont
	// have to care about this anymore.
	vstr::GetStreamManager()->AddStream( "red", *pRedStream, true );
	vstr::GetStreamManager()->AddStream( "green", *pGreenStream, true );
	// These streams can now be accessed from everywhere by calling vstr::Stream( "red" )

	// Additionally, we want to log output to a file. For this, we could create our own std::fstream
	// However, it is often more convenient to use VistaStreamManager::AddNewLogFileStream()
	// This allows to automatically add information like Time info and the cluster node name to the
	// file.
	vstr::GetStreamManager()->AddNewLogFileStream( "log", "mylogfile", "log", true, true );

	// Finally, we want our own output to show not only on the green or the log stream, but on both.
	// For this, we create another split stream
	VistaSplitOutstream* pSplitStream = new VistaSplitOutstream;
	pSplitStream->AddStream( pGreenStream );
	pSplitStream->AddStream( &vstr::Stream( "log" ) );
	// We now add it with an empty name. Since this is the default parameter for vstr::Stream(),
	// so it is a convenience for the default stream
	vstr::GetStreamManager()->AddStream( "", *pSplitStream );

	// We now write output to the new splitstream, which then appears both in the log
	// file and on the console. Additionally, VistaStreamUtils provides several
	// Stream Manipulators that allow printing things like the frameclock, systemtime, etc.
	// Especially, vstr::info outputs an information string, which by default contains the
	// frame number and the frameclock, but it can be configured using the StreamManager
	vstr::Stream() << vstr::info << "Welcome to the DebuggingToolsDemo!" << std::endl;

	// An additional tool for debugging is automatic logging of the Vista-System-Events
	// This automatically writes a line (preceded by vstr::info) to the specified stream
	// whenever an event in the mask occurs. If all events should be logged, ~0 can be passed as mask.
	VistaSystemEventLogger* pLogger = new VistaSystemEventLogger( m_pVistaSystem->GetEventManager() );
	pLogger->SetStreamEventMask( vstr::Stream( "log" ), VistaSystemEvent::VSE_POSTGRAPHICS 
												| VistaSystemEvent::VSE_UPDATE_INTERACTION );

}

DebuggingToolsDemoAppl::~DebuggingToolsDemoAppl()
{
	delete m_pVistaSystem;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void DebuggingToolsDemoAppl::Run()
{
	// Start Application
	m_pVistaSystem->Run();
}

void DebuggingToolsDemoAppl::HandleEvent( VistaEvent* pEvent )
{
	// It is often desirable to benchmark certain parts of the code.
	// While for extensive profiling external tools are the best choice, there is some simple
	// profiling built into Vista. It can be displayed during runtime by pressing 'I' (shift+i).
	// Important parts of Vista are already instrumented, but own code is sometimes of interest, too.
	// For this, manual instrumentation has to be used.
	// Note that the profiler works per frame, so only code that is called during the main loop 
	// should be instrumented. If the code is not called multiple times per frame, it depends on the
	// scope it is currently in - it forms either a new entry, or is grouped together with other calls.
	// Additionally, it is important to only profile code in the MAIN THREAD, since the implementation is
	// not thread-safe, and because it doesn't allow normalizing the data.
	// Furthermore, scopes are structured hierarchically, so it is important to always have the start
	// and end of a profile scope in the same function/scope, so that no scopes interleave.
	// In general, it is best to ensure this by using (exactly one) VistaBasicProfiler::ProfileScopeObject
	// per scope that should be profiled.
	switch( pEvent->GetId() )
	{
		case VistaSystemEvent::VSE_POSTGRAPHICS:
		case VistaSystemEvent::VSE_PREGRAPHICS:
		{
			// the ProfileScopeObject lives in the current scope, and will be
			// destroyed on the closing bracket.
			VistaBasicProfiler::ProfileScopeObject oSleepProfileScope( "SLEEP" );

			// Here, there would usually be some computation or other stuff taking
			// processor time. But since we have nothing to do, we'll just sleep
			// for 2 to 8 milliseconds
			VistaTimeUtils::Sleep( VistaRandomNumberGenerator::GetSingleton()->GenerateInt32( 2, 8 ) );

			break;
		}
		default:
			break;
	}

}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "MyDemoAppl.cpp"                                              */
/*============================================================================*/