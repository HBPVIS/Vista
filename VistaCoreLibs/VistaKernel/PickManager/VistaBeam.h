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
// $Id$

#ifndef _VISTABEAM_H
#define _VISTABEAM_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

const float VISTA_BEAM_DEFAULT_LENGTH	= 5.0f;
const float VISTA_BEAM_DEFAULT_WIDTH	= 0.01f;

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include "../EventManager/VistaEventObserver.h"
#include <VistaBase/VistaVectorMath.h>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class	VistaSG;
class	VistaGeometry;
class	VistaGeomNode;
class	VistaTransformNode;

class   IVistaTransformable;
class	VistaEvent;
class   VistaColorRGB;
class   VistaGraphicsManager;
class   VistaEventManager;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaBeam : public VistaEventObserver
{
public:
	VistaBeam(
		VistaEventManager *pEvMgr,
		VistaGraphicsManager *pGrMgr,
        IVistaTransformable *pTransform);

	virtual	~VistaBeam();

	/**
	 * Enable/Disable node
	 *
	 * @param   --
	 * @RETURN  --
	 */
	void	SetIsEnabled(bool v);

	/**
	 * checks, whether node is enabled
	 *
	 * @param   --
	 * @RETURN  bool    true/false
	 */
	bool	GetIsEnabled(void) const;

	/**
	 * Returns the origin of target beam
	 *
	 * @param   VistaVector3D p3DOrigin
	 * @RETURN  --
	 */
	void	GetOrigin(VistaVector3D &p3DOrigin) const;
	/**
	 * Sets the origin of target beam
	 *
	 * @param   VistaVector3D p3DOrigin
	 * @RETURN  --
	 */
	void	SetOrigin(const VistaVector3D &p3DOrigin);

	/**
	 * Returns the direction of target beam
	 *
	 * @param   VistaVector3D p3DDirection
	 * @RETURN  --
	 */
	void	GetDirection(VistaVector3D &p3DDirection) const;
	/**
	 * Sets the direction of target beam
	 *
	 * @param   VistaVector3D p3DDirection
	 * @RETURN  --
	 */
	void	SetDirection(const VistaVector3D &p3DDirection);

	/**
	 * Returns Orientation of target beam
	 *
	 * @param   VistaQuaternion qOrientation
	 * @RETURN  --
	 */
	void	GetOrientation(VistaQuaternion &qOrientation) const;
	/**
	 * Sets Orientation of target beam
	 *
	 * @param   VistaQuaternion qOrientation
	 * @RETURN  --
	 */
	void	SetOrientation(const VistaQuaternion &qOrientation);
	/**
	 * Returns length of target beam
	 *
	 * @param
	 * @RETURN  --
	 */
	float	GetLength(void) const;
	/**
	 * Sets lenght of target beam
	 *
	 * @param   float fLength
	 * @RETURN  --
	 */
	void	SetLength(const float &fLength);
	/**
	 * Returns width of target beam
	 *
	 * @param
	 * @RETURN  --
	 */
	float	GetWidth(void) const;
	/**
	 * Sets width of target beam
	 *
	 * @param   float fWidth
	 * @RETURN  --
	 */
	void	SetWidth(const float &fWidth);

	/**
	 * calls the update method of target beam, if the notification
	 * is about a pre-draw graphics event
	 *
	 * @param   const VistaEvent *pEvent
	 * @RETURN  --
	 */
	virtual void Notify(const VistaEvent *pEvent);

	void SetBeamColor(const VistaColorRGB &col);
	VistaColorRGB GetBeamColor() const;

	/** @deprecated */
	bool	Init(void);

protected:
	bool	BuildBeam(void);
	void	PositionNode();


private:
	VistaEventManager *m_pEvMgr;
	VistaGraphicsManager *m_pGrMgr;

	IVistaTransformable *	m_pTransform;

	VistaVector3D		m_p3DOrigin;
	VistaQuaternion	m_qOrientation;
	float				m_fLength;
	float				m_fWidth;
	bool				m_bEnabled;

	VistaTransformNode *m_pGeomTransform;
	VistaGeomNode      *m_pGeomNode;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTABEAM_H
