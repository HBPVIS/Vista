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

#ifndef _VISTAINTERACTIONCONTEXT_H
#define _VISTAINTERACTIONCONTEXT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaAspects/VistaObserveable.h>
#include <VistaBase/VistaBaseTypes.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDLVTypes.h>

#include <VistaBase/VistaVectorMath.h>

#include <VistaTools/VistaRingBuffer.h>


#include <vector>

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaDeSerializer;
class IVistaSerializer;
class VistaDeviceSensor;
class IVistaMeasureTranscode;
class VistaMeasureHistory;
class VistaInteractionManager;
class VistaInteractionEvent;
class VistaEventManager;

class VdfnGraph;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaInteractionContext : public IVistaObserveable
{
	friend class VistaInteractionManager;
public:

	VistaInteractionContext(VistaInteractionManager *pInMgr,
							 VistaEventManager *pEvMgr );

	virtual ~VistaInteractionContext();


	enum
	{
		MSG_ENABLESTATE_CHANGE = IVistaObserveable::MSG_LAST,
		MSG_UPDATE_TS_CHANGE,
        MSG_CONTEXT_GRAPH_CHANGE,
        MSG_ROLEID_CHANGE,
        MSG_GRAPHSOURCE_CHANGE,
		MSG_LAST
	};

	bool GetIsEnabled() const;
	void SetIsEnabled(bool bEnabled);

    bool GetIsEventSource() const;
    void SetIsEventSource(bool bSource);

	VdfnGraph *GetTransformGraph() const;
	void        SetTransformGraph(VdfnGraph *);

	std::string GetGraphSource() const;
	void        SetGraphSource( const std::string &strGraphFile );

    unsigned int GetUpdateIndex() const;

    unsigned int GetRoleId() const;
    void         SetRoleId( unsigned int nRoleId );


	// #######################################################################
	// SERIALIZE API
	// #######################################################################

	static int SerializeContext(IVistaSerializer &,
								const VistaInteractionContext &);
	static int DeSerializeContext(IVistaDeSerializer &,
								  VistaInteractionContext &);

	// ######################################################################
	// API for server sides contexts, users should not use this
	// ######################################################################
	bool                  Update(double dTs);
	bool                  Evaluate(double nTs);
protected:

private:

	// #######################################################################
	// COPY CONSTRUCTOR / ASSIGNMENT OPERATOR (private)
	// #######################################################################

	VistaInteractionContext(const VistaInteractionContext &)
	{}

	VistaInteractionContext &operator=(const VistaInteractionContext &)
	{
		return *this;
	}

	// #######################################################################
	// MEMBER
	// #######################################################################


	bool                   m_bEnabled,
		                   m_bRegistered,
                           m_bEventSource;

    unsigned int           m_nUpdateIndex, m_nRoleId;
	std::string             m_strGraphFile;

	VdfnGraph *m_pTransformGraph;
	VistaInteractionEvent *m_pEvent;
	VistaEventManager     *m_pEvMgr;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAINTERACTIONCONTEXT_H
