/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2011 RWTH Aachen University               */
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
// $Id: VistaFileSystemNode.cpp 22128 2011-07-01 11:30:05Z dr165799 $

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaFileSystemNode.h"

#include <sys/stat.h>
#include <sys/types.h>


#ifdef IRIX
	#include <iostream>
	#include <unistd.h>
#elif HPUX
	#include <iostream.h>
	#include <unistd.h>
#elif WIN32
	#include <Windows.h> 
	#include <iostream>
	#include <io.h>
#else
	#include <iostream>
	#include <unistd.h>
#endif

using namespace std;


/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaFileSystemNode::VistaFileSystemNode(const string &node_name) 
{
	SetName( node_name );
}


VistaFileSystemNode::~VistaFileSystemNode()
{
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

string VistaFileSystemNode::GetName() const
{
	return m_sName;
}

string VistaFileSystemNode::GetLocalName() const
{
	return m_sLocalName;
}

void VistaFileSystemNode::SetName(const string &strName)
{
	m_sName = strName;
	int iPos = m_sName.rfind( "/" );
#ifdef WIN32
	int iPos2 = m_sName.rfind( "\\" );
	if( iPos2 > iPos )
		iPos = iPos2;
#endif
	m_sLocalName = m_sName.substr( iPos + 1 );
}


VistaFileSystemNode::FILE_TYPE VistaFileSystemNode::GetType() const
{ 
	if(IsDirectory())
		return FT_DIRECTORY;
	else if(IsFile())
		return FT_FILE;
	else
		return FT_UNKNOWN; 
}


bool VistaFileSystemNode::IsReadOnly() const 
{

#ifdef WIN32  // ##### WINDOWS VERSION ########################################

	if( (_access( m_sName.c_str(), 02 )) == -1 )  // 02 == write permission
		return true; 

#else         // ##### UNIX VERSION ###########################################

	if( ( access( m_sName.c_str(), 02 )) == -1 )  // 02 == write permission
		return true; 

#endif

	return false;
}

double VistaFileSystemNode::GetCreationDate() 
{
#ifdef WIN32  // ##### WINDOWS VERSION ########################################

  struct _stat attributes;

  if ( _stat(GetName().c_str(), &attributes) != 0 )
	return 0;

  return ((double) attributes.st_ctime);

#else        // ##### UNIX VERSION ############################################

  struct stat attributes;

  if ( stat(GetName().c_str(), &attributes) != 0 )
	return 0;

  return ((double) attributes.st_ctime);

#endif
}

double VistaFileSystemNode::GetLastAccessDate() 
{
#ifdef WIN32  // ##### WINDOWS VERSION ########################################

  struct _stat attributes;

  if ( _stat(GetName().c_str(), &attributes) != 0 )
	return 0;

  return ((double) attributes.st_atime);

#else        // ##### UNIX VERSION ############################################

  struct stat attributes;

  if ( stat(GetName().c_str(), &attributes) != 0 )
	return 0;

  return ((double) attributes.st_atime);

#endif
}

double VistaFileSystemNode::GetLastModifiedDate() 
{
#ifdef WIN32  // ##### WINDOWS VERSION ########################################

  struct _stat attributes;

  if ( _stat(GetName().c_str(), &attributes) != 0 )
	return 0;

  return ((double) attributes.st_mtime);

#else        // ##### UNIX VERSION ############################################

  struct stat attributes;

  if ( stat(GetName().c_str(), &attributes) != 0 )
	return 0;

  return ((double) attributes.st_mtime);

#endif
}

bool VistaFileSystemNode::IsDirectory() const
{
#ifndef WIN32
  struct stat status;
  return Exists() && (::stat(GetName().c_str(),&status)==0) && S_ISDIR(status.st_mode);
#else
  DWORD atts;
  return Exists() && ((atts=GetFileAttributes(GetName().c_str()))!=0xFFFFFFFF) && (atts&FILE_ATTRIBUTE_DIRECTORY);
#endif
}

bool VistaFileSystemNode::IsFile() const
{
#ifndef WIN32
  struct stat status;
  return Exists() && (::stat(GetName().c_str(),&status)==0) && S_ISREG(status.st_mode);
#else
  DWORD atts;
  return Exists() && ((atts=GetFileAttributes(GetName().c_str()))!=0xFFFFFFFF) && !(atts&FILE_ATTRIBUTE_DIRECTORY);
#endif
}
