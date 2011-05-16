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

#include "VistaProjection.h"
#include "VistaDisplayBridge.h"
#include "VistaViewport.h"
#include "VistaDisplayManager.h"
#include "VistaDisplaySystem.h"

#include <VistaKernel/VistaKernelOut.h>

#include <VistaAspects/VistaAspectsUtils.h>

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaProjection::VistaProjection(VistaViewport *pViewport, 
								   IVistaDisplayEntityData *pData,
								   IVistaDisplayBridge *pBridge)
: VistaDisplayEntity(pData, pBridge), m_pViewport(pViewport)
{
}

VistaProjection::~VistaProjection()
{
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewport                                                 */
/*                                                                            */
/*============================================================================*/
VistaViewport *VistaProjection::GetViewport() const
{
	return m_pViewport;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportName                                             */
/*                                                                            */
/*============================================================================*/
std::string VistaProjection::GetViewportName() const
{
	string sViewportName;

	if(m_pViewport)
	{
		sViewportName = m_pViewport->GetNameForNameable();
	}
	return sViewportName;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaProjection::Debug(std::ostream &out) const
{
	VistaDisplayEntity::Debug(out);
	VistaVector3D v3MidPoint, v3NormalVector, v3UpVector;
	GetProjectionProperties()->GetProjectionPlane(v3MidPoint, v3NormalVector, v3UpVector);
	out << " [VistaProjection] - plane midpoint:      " << v3MidPoint << endl;
	out << " [VistaProjection] - plane normal:        " << v3NormalVector << endl;
	out << " [VistaProjection] - plane up vector:     " << v3UpVector << endl;

	double dLeft, dRight, dBottom, dTop, dNear, dFar;
	GetProjectionProperties()->GetProjPlaneExtents(dLeft, dRight, dBottom, dTop);
	out << " [VistaProjection] - plane extents (l/r): " << dLeft << " / " << dRight << endl;
	out << " [VistaProjection] - plane extents (b/t): " << dBottom << " / " << dTop << endl;

	GetProjectionProperties()->GetClippingRange(dNear, dFar);
	out << " [VistaProjection] - clipping range:      " << dNear << " / " << dFar << endl;

	out << " [VistaProjection] - stereo mode:         " 
		<< GetProjectionProperties()->GetStereoModeString() << endl;

	out << " [VistaProjection] - viewport name:       ";
	if(m_pViewport)
	{
		out << m_pViewport->GetNameForNameable() << endl;
	}
	else
	{
		out << "*none* (no viewport given)" << endl;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateProperties                                            */
/*                                                                            */
/*============================================================================*/
VistaDisplayEntity::IVistaDisplayEntityProperties *VistaProjection::CreateProperties()
{
	return new VistaProjectionProperties(this, GetDisplayBridge());
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetProjecionProperties                                      */
/*                                                                            */
/*============================================================================*/
VistaProjection::VistaProjectionProperties *VistaProjection::GetProjectionProperties() const
{
	return static_cast<VistaProjectionProperties*>(GetProperties());
}


namespace {
	const std::string sSReflectionType("VistaProjection");

	IVistaPropertyGetFunctor *aCgFunctors[] =
	{
		new TVistaProperty3RefGet<float, VistaProjection::VistaProjectionProperties, VistaProperty::PROPT_DOUBLE>
		("PROJ_PLANE_MIDPOINT", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::GetProjPlaneMidpoint),	
		new TVistaProperty3RefGet<float, VistaProjection::VistaProjectionProperties, VistaProperty::PROPT_DOUBLE>
		("PROJ_PLANE_NORMAL", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::GetProjPlaneNormal),	
		new TVistaProperty3RefGet<float, VistaProjection::VistaProjectionProperties, VistaProperty::PROPT_DOUBLE>
		("PROJ_PLANE_UP", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::GetProjPlaneUp),	
		new TVistaProperty4RefGet<double, VistaProjection::VistaProjectionProperties, VistaProperty::PROPT_DOUBLE>
		("PROJ_PLANE_EXTENTS", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::GetProjPlaneExtents),	
		new TVistaProperty2RefGet<double, VistaProjection::VistaProjectionProperties, VistaProperty::PROPT_DOUBLE>
		("CLIPPING_RANGE", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::GetClippingRange),	
		new TVistaPropertyGet<std::string, VistaProjection::VistaProjectionProperties, VistaProperty::PROPT_STRING>
		("STEREO_MODE", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::GetStereoModeString),
		new TVistaDisplayEntityParentPropertyGet<std::string, VistaProjection, VistaProperty::PROPT_STRING>
		("VIEWPORT_NAME", sSReflectionType,
		 &VistaProjection::GetViewportName),
		NULL
	};

	IVistaPropertySetFunctor *aCsFunctors[] =
	{
		new TVistaProperty3ValSet<float, VistaProjection::VistaProjectionProperties>
		("PROJ_PLANE_MIDPOINT", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::SetProjPlaneMidpoint,
		 &VistaAspectsConversionStuff::ConvertStringTo3Float),
		new TVistaProperty3ValSet<float, VistaProjection::VistaProjectionProperties>
		("PROJ_PLANE_NORMAL", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::SetProjPlaneNormal,
		 &VistaAspectsConversionStuff::ConvertStringTo3Float),
		new TVistaProperty3ValSet<float, VistaProjection::VistaProjectionProperties>
		("PROJ_PLANE_UP", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::SetProjPlaneUp,
		 &VistaAspectsConversionStuff::ConvertStringTo3Float),
		new TVistaProperty4ValSet<double, VistaProjection::VistaProjectionProperties>
		("PROJ_PLANE_EXTENTS", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::SetProjPlaneExtents,
		 &VistaAspectsConversionStuff::ConvertStringTo4Double),
		new TVistaProperty2ValSet<double, VistaProjection::VistaProjectionProperties>
		("CLIPPING_RANGE", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::SetClippingRange,
		 &VistaAspectsConversionStuff::ConvertStringTo2Double),
		new TVistaPropertySet<const std::string &, std::string, VistaProjection::VistaProjectionProperties>
		("STEREO_MODE", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::SetStereoModeString,
		 &VistaAspectsConversionStuff::ConvertToString),
		new TVistaPropertySet<const string &, string,VistaProjection::VistaProjectionProperties>
		("NAME", sSReflectionType,
		 &VistaProjection::VistaProjectionProperties::SetName,
		 &VistaAspectsConversionStuff::ConvertToString),
		NULL
	};
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetName                                                     */
/*                                                                            */
/*============================================================================*/
bool VistaProjection::VistaProjectionProperties::SetName(const std::string &sName)
{
	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	string strOldName = pP->GetNameForNameable();

	if( strOldName != "")
	{
		if (!pP->GetViewport()->GetDisplaySystem()->GetDisplayManager()->RenameProjection(strOldName, sName))
			return false;
	}
	return IVistaDisplayEntityProperties::SetName(sName);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetProjPlaneMidpoint                                    */
/*                                                                            */
/*============================================================================*/
bool VistaProjection::VistaProjectionProperties::GetProjPlaneMidpoint(float &x, float &y, float &z) const
{
	VistaVector3D v3Mid, v3Normal, v3Up;

	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	GetDisplayBridge()->GetProjectionPlane(v3Mid, v3Normal, v3Up, pP);

	x = v3Mid[0];
	y = v3Mid[1];
	z = v3Mid[2];

	return true;
}

bool VistaProjection::VistaProjectionProperties::SetProjPlaneMidpoint(const float x, const float y, const float z)
{
	VistaVector3D v3Mid, v3Normal, v3Up;
	VistaVector3D v3NewMid(x,y,z);

	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	GetDisplayBridge()->GetProjectionPlane(v3Mid, v3Normal, v3Up, pP);

	if(v3Mid == v3NewMid) return false;
	else
	{
		GetDisplayBridge()->SetProjectionPlane(v3NewMid, v3Normal, v3Up, pP);
		Notify( MSG_PROJ_PLANE_MIDPOINT_CHANGE );
		return true;
	}

}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetProjPlaneNormal                                      */
/*                                                                            */
/*============================================================================*/
bool VistaProjection::VistaProjectionProperties::GetProjPlaneNormal(float &x, float &y, float &z) const
{
	VistaVector3D v3Mid, v3Normal, v3Up;

	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	GetDisplayBridge()->GetProjectionPlane(v3Mid, v3Normal, v3Up, pP);

	x = v3Normal[0];
	y = v3Normal[1];
	z = v3Normal[2];

	return true;
}

bool VistaProjection::VistaProjectionProperties::SetProjPlaneNormal(const float x, const float y, const float z)
{
	VistaVector3D v3Mid, v3Normal, v3Up;
	VistaVector3D v3NewNormal(x,y,z);

	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	GetDisplayBridge()->GetProjectionPlane(v3Mid, v3Normal, v3Up, pP);

	if(v3Normal == v3NewNormal) return false;
	else
	{
		GetDisplayBridge()->SetProjectionPlane(v3Mid, v3NewNormal, v3Up, pP);
		Notify( MSG_PROJ_PLANE_NORMAL_CHANGE );
		return true;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetProjPlaneUp                                          */
/*                                                                            */
/*============================================================================*/
bool VistaProjection::VistaProjectionProperties::GetProjPlaneUp(float &x, float &y, float &z) const
{
	VistaVector3D v3Mid, v3Normal, v3Up;

	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	GetDisplayBridge()->GetProjectionPlane(v3Mid, v3Normal, v3Up, pP);

	x = v3Up[0];
	y = v3Up[1];
	z = v3Up[2];

	return true;
}

bool VistaProjection::VistaProjectionProperties::SetProjPlaneUp(const float x, const float y, const float z)
{
	VistaVector3D v3Mid, v3Normal, v3Up;
	VistaVector3D v3NewUp(x,y,z);

	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	GetDisplayBridge()->GetProjectionPlane(v3Mid, v3Normal, v3Up, pP);

	if(v3Up == v3NewUp) return false;
	else
	{
		GetDisplayBridge()->SetProjectionPlane(v3Mid, v3Normal, v3NewUp, pP);
		Notify( MSG_PROJ_PLANE_UP_CHANGE );
		return true;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetProjPlaneExtents                                     */
/*                                                                            */
/*============================================================================*/
bool VistaProjection::VistaProjectionProperties::GetProjPlaneExtents(double &dLeft, double &dRight, double &dBottom, double &dTop) const
{
	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	GetDisplayBridge()->GetProjPlaneExtents(dLeft, dRight, dBottom, dTop, pP);

	return true;
}

bool VistaProjection::VistaProjectionProperties::SetProjPlaneExtents(const double dLeft, const double dRight, const double dBottom, const double dTop)
{
	double dCurLeft, dCurRight, dCurBottom, dCurTop;

	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	GetDisplayBridge()->GetProjPlaneExtents(dCurLeft, dCurRight, dCurBottom, dCurTop, pP);

	if( (dCurLeft == dLeft) && (dCurRight == dRight) && (dCurBottom == dBottom) && (dCurTop == dTop)) return false;
	else
	{
		GetDisplayBridge()->SetProjPlaneExtents(dLeft, dRight, dBottom, dTop, pP);
		Notify( MSG_PROJ_PLANE_EXTENTS_CHANGE );
		return true;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetClippingRange                                        */
/*                                                                            */
/*============================================================================*/
bool VistaProjection::VistaProjectionProperties::GetClippingRange(double &dNear, double &dFar) const
{
	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	GetDisplayBridge()->GetProjClippingRange(dNear, dFar, pP);
	return true;

}

bool VistaProjection::VistaProjectionProperties::SetClippingRange(const double dNear, const double dFar)
{
	double dCurNear, dCurFar;

	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	GetDisplayBridge()->GetProjClippingRange(dCurNear, dCurFar, pP);

	if( (dCurNear == dNear) && (dCurFar == dFar) ) return false;
	else
	{
		GetDisplayBridge()->SetProjClippingRange(dNear, dFar, pP);
		Notify( MSG_CLIPPING_RANGE_CHANGE );
		return true;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetStereoMode                                           */
/*                                                                            */
/*============================================================================*/
int VistaProjection::VistaProjectionProperties::GetStereoMode() const
{
	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	return GetDisplayBridge()->GetProjStereoMode(pP);
}

bool VistaProjection::VistaProjectionProperties::SetStereoMode(int iMode)
{
	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	

	if( GetDisplayBridge()->GetProjStereoMode(pP) == iMode )
		return false;
	else
	{
		GetDisplayBridge()->SetProjStereoMode(iMode, pP);
		Notify( MSG_STEREO_MODE_CHANGE );
		return true;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetStereoModeString                                     */
/*                                                                            */
/*============================================================================*/
std::string VistaProjection::VistaProjectionProperties::GetStereoModeString() const
{
	int iMode = GetStereoMode();

	switch (iMode)
	{
	case SM_MONO:
		return "MONO";
	case SM_LEFT_EYE:
		return "LEFT_EYE";
	case SM_RIGHT_EYE:
		return "RIGHT_EYE";
	case SM_FULL_STEREO:
		return "FULL_STEREO";
	}

	vkernout << " [VistaProjection] - WARNING - unable to determine stereo mode (" 
		 << iMode << ")..." << endl;

	return "";
}

bool VistaProjection::VistaProjectionProperties::SetStereoModeString(const std::string &strMode)
{
	int iMode = SM_MONO;
	string strModeUpper = VistaAspectsConversionStuff::ConvertToUpper(strMode);

	if (strModeUpper == "MONO")
		iMode = SM_MONO;
	else if (strModeUpper == "LEFT_EYE")
		iMode = SM_LEFT_EYE;
	else if (strModeUpper == "RIGHT_EYE")
		iMode = SM_RIGHT_EYE;
	else if (strModeUpper == "FULL_STEREO")
		iMode = SM_FULL_STEREO;
	else
	{
		vkernout << " [VistaProjection] - WARNING - unknown stereo mode '" << strMode << "'..." << endl;
		vkernout << "                               defaulting to 'MONO'..." << endl;
		iMode = SM_MONO;
	}

	return SetStereoMode(iMode);
}

/*============================================================================*/
/*  CONVENIENCE API BEGIN                                                     */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetProjectionPlane                                      */
/*                                                                            */
/*============================================================================*/
bool VistaProjection::VistaProjectionProperties::SetProjectionPlane(
	const VistaVector3D &v3MidPoint,
	const VistaVector3D &v3NormalVector,
	const VistaVector3D &v3UpVector)
{
	bool ret = false;

	if( SetProjPlaneMidpoint( v3MidPoint[0], v3MidPoint[1], v3MidPoint[2] ) == true )
		ret = true;

	if( SetProjPlaneNormal( v3NormalVector[0], v3NormalVector[1], v3NormalVector[2] ) == true )
		ret = true;

	if( SetProjPlaneUp( v3UpVector[0], v3UpVector[1], v3UpVector[2] ) == true )
		ret = true;

	return ret;
}

void VistaProjection::VistaProjectionProperties::GetProjectionPlane(
	VistaVector3D &v3MidPoint, 
	VistaVector3D &v3NormalVector,
	VistaVector3D &v3UpVector) const
{
	VistaProjection *pP = static_cast<VistaProjection*>(GetParent());	
	GetDisplayBridge()->GetProjectionPlane(v3MidPoint, v3NormalVector, v3UpVector, pP);
}

/*============================================================================*/
/*  CONVENIENCE API END                                                       */
/*============================================================================*/


string VistaProjection::VistaProjectionProperties::GetReflectionableType() const
{
	return sSReflectionType;
}

int VistaProjection::VistaProjectionProperties::AddToBaseTypeList(list<string> &rBtList) const
{
	int nSize = IVistaDisplayEntityProperties::AddToBaseTypeList(rBtList);
	rBtList.push_back(sSReflectionType);
	return nSize + 1;
}

VistaProjection::VistaProjectionProperties::~VistaProjectionProperties()
{
}

VistaProjection::VistaProjectionProperties::VistaProjectionProperties(
	VistaProjection *pParent, 
	IVistaDisplayBridge *pDisplayBridge)
  : IVistaDisplayEntityProperties(pParent, pDisplayBridge)
{
}


