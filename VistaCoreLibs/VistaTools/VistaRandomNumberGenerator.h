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

#ifndef _VISTARANDOMNUMBERGENERATOR_H
#define _VISTARANDOMNUMBERGENERATOR_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
//#define N 624

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTATOOLSAPI VistaRandomNumberGenerator
{
public:
	static VistaRandomNumberGenerator *GetSingleton();

	void          SetSeed(unsigned int);
	void          SetSeedByArray(unsigned int init_key[], int key_length);

	unsigned int GenerateInt32() ;

	/**
	* [iLowerBound, iUpperBound]
	*/
	unsigned int GenerateInt32(unsigned int iLowerBound, unsigned int iUpperBound);

	int           GenerateInt31() ;

	/**
	* [0,1)
	*/
	double        GenerateDouble1() ;
	/**
	* [0,1]
	*/
	double        GenerateDouble2() ;

	/**
	* (0,1)
	*/
	double        GenerateDouble3() ;


	/**
	* [0,1), 53bit resolution
	*/
	double        GenerateDouble53() ;


protected:
private:
	VistaRandomNumberGenerator();
	~VistaRandomNumberGenerator();


	//MEMBERS
	//unsigned long m_mt[N]; /* the array for the state vector  */
	unsigned int *m_mt;
	int m_mti; //=N+1; /* mti==N+1 means mt[N] is not initialized */

	static VistaRandomNumberGenerator *s_pSingleton;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTARANDOMNUMBERGENERATOR_H

