/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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

#ifndef _VISTADTRACKCOMMONSHARE_H
#define _VISTADTRACKCOMMONSHARE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

namespace VistaDTrackMeasures
{
	/**
	 * memory layout of the global type sensor
	 */
	struct sGlobalMeasure
	{
		int   m_nFrameCount;
		float m_nTimeStamp;
	};


	/**
	 * memory layout of the typical body measure
	 */
	struct sBodyMeasure
	{
		double  m_nId;
		double m_nQuality;
		double  m_nPos[3],
				m_nEuler[3],
				m_anRot[9];
	};


	/**
	 * memory layout of the stick measure
	 */
	struct sStickMeasure
	{
		double  m_nId;
		double m_nQuality;
		double m_nButtonState;
		double  m_nPos[3],
				m_nEuler[3],
				m_anRot[9];
	};

	/**
	 *  memory layout of the marker measure
	 */
	struct sMarkerMeasure
	{
		double m_nId;
		double m_nQuality,
			  m_nPos[3];
	};

	/**
	 * memory layout of the measure device's measure
	 */
	struct sMeasureMeasure
	{
		double m_nId;
		double m_nQuality;
		double m_nButtonState;
		double m_nPos[3],
			   m_anRot[9];
	};

	/**
	 * memory layout of the base measure
	 */
	struct sBaseMasure
	{
		double m_nId,
			   m_nQuality;
	};

	/**
	 * this is a dummy structure to read off the values in a generic
	 * way. the other structures are simple access functions for the
	 * transcode getters!
	 */
	struct sGenericMeasure
	{
		double m_anField[18];
	};

}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADTRACKCOMMONSHARE_H
