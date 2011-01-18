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
// $Id$


#ifdef WIN32

#endif
#ifdef LINUX
#include <linux/input.h>
//#include <sys/types.h>

#include <dirent.h>
#include <fcntl.h>

// dirty hack for the messed up kernel headers of opensuse 10.2.
// this should be removed as soon as recent SuSE installations ship
// correct kernel headers, i.e. cleaned of the "sparse" __user etc.
// annotations. (http://www.kernel.org/pub/software/devel/sparse)
#ifndef __user
  #define __user
#endif

#include <linux/usbdevice_fs.h>
#include <asm/byteorder.h>
#include <cerrno>

#include <cstring>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <iterator>

#include <cstdlib>

#include <regex.h>
#endif

//#include <VistaTools/VistaFileSystemDirectory.h>

#include "VistaConnectionFile.h"
#include "VistaConnectionUSB.h"

#include <VistaInterProcComm/VistaInterProcCommOut.h>

using namespace std;
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaConnectionUSB::VistaConnectionUSB( unsigned int vendorid,
										  unsigned int productid ) :
	m_nVendorID(vendorid),
	m_nProductID(productid),
	m_pFileCon(NULL)
{
	SetIsOpen(false);
}

VistaConnectionUSB::~VistaConnectionUSB()
{
	Close();
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


bool VistaConnectionUSB::Open()
{
	if(GetIsOpen())
		return true;

#ifdef WIN32

#endif
#ifdef LINUX
/*
	// inspect /dev/bus/usb/devices using a very crappy "parser"
	// to find a  device with desired vendor/product id

	std::ifstream sInput("/dev/bus/usb/devices");
	std::string strLine;

	int busid, devnum=0;
	int vendorid, productid=0;
	while( std::getline(sInput, strLine))
	{
		if( strLine == "" )
		{
			if( busid != 0 && devnum != 0 )
			{
#ifdef DEBUG
				vipcout << std::hex << "found USB device with busid " << busid
						  << ", device number " << devnum 
						  << ", vendorid " << vendorid
						  << ", product id " << productid
						  << std::endl;
#endif

				if( vendorid == m_nVendorID && productid == m_nProductID )
				{
					char szDevPath[128];
					sprintf( szDevPath, "/dev/bus/usb/%03d/%03d", busid, devnum ); 
					std::string strDevPath( szDevPath );
					
#ifdef DEBUG
					vipcout << "[ConnectionUSB] device with vendorid/productid "
							  << vendorid << "/" << productid
							  << " found, opening file connection on device path "
							  << strDevPath << std::endl;

#endif
					
					m_pFileCon = new VistaConnectionFile( strDevPath,
														   VistaConnectionFile::READ );

					if( !m_pFileCon->Open() )
					{
						vipcerr << "[ConnectionUSB] failed to open file connection!" << std::endl;
						return false;
					}
					
					if( ioctl( m_pFileCon->GetConnectionWaitForDescriptor(), USBDEVFS_CLAIMINTERFACE ) )
					{
						perror( "[ConnectionUSB] error claiming usb interface" );
					}


					return true;
				}
			}

			busid=0;
			devnum=0;
			vendorid=0;
			productid=0;
		}
		else if( strLine[0] == 'T' )
		{
			int pos;
			std::string strConv;

			pos = strLine.find( "Bus=" );
			strConv = "";
			strConv += strLine[pos+4];
			strConv += strLine[pos+5];

			busid = strtol( strConv.c_str(), NULL, 16 );
			
			pos = strLine.find( "Dev#=" );
			strConv = "";
			strConv += strLine[pos+5];
			strConv += strLine[pos+6];
			strConv += strLine[pos+7];

			devnum = strtol( strConv.c_str(), NULL, 16 );
		}
		else if( strLine[0] == 'P' )
		{
			int pos;
			std::string strConv;
			
			pos = strLine.find( "Vendor" );
			strConv = "";
			strConv += strLine[pos+7];
			strConv += strLine[pos+8];
			strConv += strLine[pos+9];
			strConv += strLine[pos+10];
			
			vendorid = strtol( strConv.c_str(), NULL, 16 );

			pos = strLine.find( "ProdID" );
			strConv = "";
			strConv += strLine[pos+7];
			strConv += strLine[pos+8];
			strConv += strLine[pos+9];
			strConv += strLine[pos+10];

			productid = strtol( strConv.c_str(), NULL, 16 );
		}
	}
*/

	// peek into all files in /dev/bus/usb/*/* (usb device descriptors)
	// and try to find matching vendor and product id.
/*	VistaFileSystemDirectory oUsbDir( "/proc/bus/usb" );

	for( VistaFileSystemDirectory::iterator it = oUsbDir.begin() ; 
		 it != oUsbDir.end() ; 
		 it++ )
	{
		regex_t expr;
		// check if filename is three hexadecimal digits
		regcomp( &expr, "[A-Fa-f0-9][A-Fa-f0-9][A-Fa-f0-9]", 0 );
		
		if( regexec( &expr, (*it)->GetName().c_str(), 0, NULL, 0 ) )
			continue;


		VistaFileSystemDirectory oBusDir( (*it)->GetName() );

		for( VistaFileSystemDirectory::iterator busit = oBusDir.begin() ; 
			 busit != oBusDir.end() ; 
			 busit++ )
		{
			if( regexec( &expr, (*busit)->GetName().c_str(), 0, NULL, 0 ) )
				continue;

#ifdef DEBUG
			vipcout << "[ConnectionUSB] reading device descriptor from " 
					  << (*busit)->GetName() << std::endl;
#endif

			m_pFileCon = new VistaConnectionFile( (*busit)->GetName(),
												   VistaConnectionFile::READWRITE );
			
			if( !m_pFileCon->Open() )
			{
				vipcerr << "[ConnectionUSB] failed to open fileconnection!" << std::endl;
			}

			unsigned char device_descriptor[18];
			if(	m_pFileCon->Receive( device_descriptor, 18, 0 ) != 18 )
			{
				vipcout << "[ConnectionUSB] unable to read 18 byte device descriptor" << std::endl;
				m_pFileCon->Close();
				delete m_pFileCon;
				continue;
			}

#ifdef DEBUG
			ios_base::fmtflags format = vipcout.flags();
			for( int i = 0 ; i < 18 ; i++ )
			{
				vipcout << std::hex << std::setfill('0') << std::setw(2)
						  << (unsigned int)(device_descriptor[i]) << " " ;
			}
			vipcout << std::endl;
			vipcout.flags(format);			
#endif

			// bytes come from the USB bus LSB first, thus switch and bitshift
			unsigned int productid = 
				device_descriptor[10] + (device_descriptor[11]<<8);
			unsigned int vendorid = 
				device_descriptor[8] + (device_descriptor[9]<<8);

			if( vendorid == m_nVendorID && productid == m_nProductID )
			{
				vipcout << std::hex << "device with vendorid/productid "
						  << vendorid << "/" << productid
						  << " found. keeping fileconnection open." 
						  << std::endl << std::dec;

				// for now, we claim the first usb interface (0) hardcoded
				// all simple devices exhibit only one interface anyways.
				// this could be passed as creation argument (e.g. via ini-section)
				// if required.
				int config=0;
				if( ioctl( m_pFileCon->GetConnectionWaitForDescriptor(), USBDEVFS_CLAIMINTERFACE, &config ) )
				{
					perror( "[ConnectionUSB] error claiming usb interface" );
					vipcout << "errno: " << errno << std::endl;
					vipcout << "ESPIPE: " << ESPIPE << std::endl;
					delete m_pFileCon;
					continue;
				}

				SetIsOpen(true);
				return true;
			}
			
			delete m_pFileCon;
		}
	}
*/
#endif

/*	std::cout << std::hex 
			  << "[ConnectionUSB] no device with vendor/product id " 
			  << m_nVendorID << "/" << m_nProductID
			  << " detected. failed to open usb connection." << std::endl;
*/

	vipcout << "[ConnectionUSB] Open: not implemented on this platform."
			  << std::endl;

	SetIsOpen(false);
	return false;
}

void VistaConnectionUSB::Close()
{
	if( !GetIsOpen() )
		return; 

	m_pFileCon->Close();
}

int VistaConnectionUSB::Receive(void *buffer, const int length, int iTimeout)
{
	if( !GetIsOpen() )
		return -1;

	int ret = 0;
#ifdef LINUX
	usbdevfs_bulktransfer transfer;
	
	// bulk read from the first USB_DIR_IN (input) descriptor, 0x81
	//
	// do we need a real USB-conform api?
	// we would have to distinguish:
	// control, bulk, interrupt, and isochronous transfer modes
	// different usb configurations, interfaces, and alternate settings
	// devices with multiple endpoints
	// synchronous/asynchronous I/O
	//
	// most devices, however, have only one interrupt IN endpoint,
	// one configuration, interface, and alternate setting, so this
	// will do for now.
	transfer.ep = 0x81;

	transfer.len = length;
	transfer.timeout = iTimeout;

	// error handling!
	ret = ioctl( m_pFileCon->GetConnectionWaitForDescriptor(), 
					 USBDEVFS_BULK,
					 &transfer );

	if( ret < 0 )
	{
		perror( "[ConnectionUSB] ioctl error" );
		vipcerr << "error code: " << ret << std::endl;
		return -1;
	}	
#endif	

	return ret;
}

int VistaConnectionUSB::Send(const void *buffer, const int length)
{
	if( !GetIsOpen() )
		return -1;

	return m_pFileCon->Send( buffer, length );
}

bool VistaConnectionUSB::HasPendingData() const 
{
	return m_pFileCon->HasPendingData();
}

unsigned long VistaConnectionUSB::PendingDataSize() const
{
	return m_pFileCon->PendingDataSize();
}

HANDLE VistaConnectionUSB::GetConnectionDescriptor() const
{
	return m_pFileCon->GetConnectionDescriptor();
}

HANDLE VistaConnectionUSB::GetConnectionWaitForDescriptor()
{
	return m_pFileCon->GetConnectionWaitForDescriptor();
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

