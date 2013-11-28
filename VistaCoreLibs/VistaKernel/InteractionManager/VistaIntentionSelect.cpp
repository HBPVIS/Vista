/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

#include "VistaIntentionSelect.h"

#include <functional>
#include <algorithm>
#include <VistaKernel/GraphicsManager/VistaNodeInterface.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool VistaNodeAdapter::GetPosition(VistaVector3D &pTrans) const
{
	return m_pNode->GetWorldPosition(pTrans);
}

IVistaNode* VistaNodeAdapter::GetNode() const
{
	return m_pNode;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class VistaIntentionSelect::_PrNodeEqual : public std::unary_function<_sObjectScore*, bool>
{
public:
	_PrNodeEqual(IVistaIntentionSelectAdapter *pNode)
		: m_pNode(pNode)
	{
	}

	bool operator()(_sObjectScore &obj) const
	{
		return (m_pNode == obj.m_pNode);
	}

private:
	IVistaIntentionSelectAdapter *m_pNode;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaIntentionSelect::VistaIntentionSelect()
: m_oCone(),
  m_nStickyness(0.5f),
  m_nSnappiness(0.5f)
{
}

VistaIntentionSelect::~VistaIntentionSelect()
{
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaIntentionSelect::SetSelectionVolume( const VistaEvenCone &cone )
{
	m_oCone = cone;
}

VistaEvenCone VistaIntentionSelect::GetSelectionVolume() const
{
	return m_oCone;
}

void VistaIntentionSelect::RegisterNode(IVistaIntentionSelectAdapter *pNode)
{
	m_vecScores.push_back( _sObjectScore(pNode) );
}

bool VistaIntentionSelect::UnregisterNode(IVistaIntentionSelectAdapter *pNode)
{

	NDVEC::iterator it = std::remove_if( m_vecScores.begin(),
										 m_vecScores.end(),
										 _PrNodeEqual(pNode) );

	m_vecScores.erase( it, m_vecScores.end() );

	return true;
}


void VistaIntentionSelect::Update( std::vector<IVistaIntentionSelectAdapter*>& vecObj )
{
	VistaVector3D v3Point;

	// do the scoring on all registered nodes
	for(NDVEC::iterator it = m_vecScores.begin();
		it != m_vecScores.end(); ++it)
	{
		IVistaIntentionSelectAdapter *pPointNode = (*it).m_pNode;
		if (!pPointNode->GetIsSelectionEnabled())
		{
			// set to zero score, so object will not be in vecObj
			(*it).m_nScore = 0.0;
		}
		else
		{
			// regardless of the handle type we are using, we need the distance to the first point
			VistaVector3D v3Point1;

			pPointNode->GetPosition(v3Point1);
			float fContrib = CalculatePointContribution(v3Point1);

			// if we are using a line handle, we also need the distance to the second point and to the line
			IVistaIntentionSelectLineAdapter* pLineNode = 
				dynamic_cast<IVistaIntentionSelectLineAdapter*>(pPointNode);
			if (pLineNode)
			{
				// calculate contribution of the second point
				VistaVector3D v3Point2;

				pLineNode->GetPosition2(v3Point2);
				float fContrib2 = CalculatePointContribution(v3Point2);

				// use the higher contribution of both points
				fContrib = std::max(fContrib, fContrib2);

				// calculate the contribution of the closest point between ray and line
				VistaVector3D v3Point3 = CalculateClosestPointToRay(v3Point1, v3Point2);
				float fContrib3 = CalculatePointContribution(v3Point3);

				// use the highest contribution
				fContrib = std::max(fContrib, fContrib3);
			}

			(*it).m_nScore = std::max<float>(0.0f, ((*it).m_nScore * m_nStickyness) + (fContrib * m_nSnappiness));
		}
	}

	// the std::sort() takes the predicate operator
	// as defined in _sObjectScore, which means that the
	// elements are sorted descending on their score
	// however, the sorting is not stable, so little jitter might
	// occur at the gain of faster sorting.
	std::sort(m_vecScores.begin(), m_vecScores.end());

	// Might be little bit faster to allocate memory before loop
	if( vecObj.size() != m_vecScores.size() )
		vecObj.reserve(m_vecScores.size());
	// push the nodes to the vector that was passed by the user
	// (assumed to be empty)
	for(NDVEC::iterator cit = m_vecScores.begin();
		cit != m_vecScores.end(); ++cit)
	{
		if( (*cit).m_nScore > 0 )
			vecObj.push_back( (*cit).m_pNode );
	}
}


void VistaIntentionSelect::SetConeTransform( const VistaVector3D &v3Pos,
						   const VistaQuaternion &qOri)
{
	m_ConeRef.SetTranslation(v3Pos);
	m_ConeRef.SetRotation(qOri);
}

int  VistaIntentionSelect::GetNodeCount() const
{
	return (int)m_vecScores.size();
}

IVistaIntentionSelectAdapter *VistaIntentionSelect::GetNodeByIndex(int nIndex) const
{
	if( nIndex > static_cast<int>(m_vecScores.size()) )
		return NULL;
	return m_vecScores[nIndex].m_pNode;
}

float VistaIntentionSelect::GetNodeScore(IVistaIntentionSelectAdapter *pNode) const
{
	for(NDVEC::const_iterator it = m_vecScores.begin();
		it != m_vecScores.end(); ++it)
	{
		if( (*it).m_pNode == pNode )
			return (*it).m_nScore;
	}
	return 0.0f;
}

void VistaIntentionSelect::SetStickyness( float nStickyness )
{
	if(nStickyness <= 1.0 && nStickyness >= 0.0f)
	{
		m_nStickyness = nStickyness;
		CleanScores();
	}
}

float VistaIntentionSelect::GetStickyness() const
{
	return m_nStickyness;
}


void VistaIntentionSelect::SetSnappiness(float nSnappiness)
{
	if(nSnappiness <= 1.0 && nSnappiness >= 0.0f)
	{
		m_nSnappiness = nSnappiness;
		CleanScores();
	}
}

float VistaIntentionSelect::GetSnappiness() const
{
	return m_nSnappiness;
}

void VistaIntentionSelect::CleanScores()
{
	for(NDVEC::iterator cit = m_vecScores.begin();
		cit != m_vecScores.end(); ++cit)
	{
		(*cit).m_nScore = 0.0f;
	}
}

float VistaIntentionSelect::CalculatePointContribution( const VistaVector3D & v3Point )
{
	VistaVector3D v3TransformedPoint = m_ConeRef.TransformPositionToFrame(v3Point);

	float fPerp = ::sqrtf( 
		v3TransformedPoint[Vista::X]*v3TransformedPoint[Vista::X]
	+ v3TransformedPoint[Vista::Y]*v3TransformedPoint[Vista::Y]
	);

	float fProj = -v3TransformedPoint[Vista::Z];

	return CalculateContribution( fPerp, fProj );
}

float VistaIntentionSelect::CalculateContribution( float fPerp, float fProj )
{
	float fContrib;

	if( fProj > 0 )
	{
		fContrib = 1 - ( ::atanf( fPerp / ::powf( fProj, 4.0f/5.0f ) ) / m_oCone.GetOpeningAngle() );
	}
	else
	{
		fContrib = 0;
	}

	return fContrib;
}

VistaVector3D VistaIntentionSelect::CalculateClosestPointToRay( VistaVector3D v3Point1, VistaVector3D v3Point2 )
{
	// finds the closest point between the pick ray and the line defined by point 1 and 2

	// work in the reference frame of the pick ray
	VistaVector3D v3Point1Transformed( m_ConeRef.TransformPositionToFrame( v3Point1 ) );
	VistaVector3D v3Point2Transformed( m_ConeRef.TransformPositionToFrame( v3Point2 ) );

	VistaVector3D v3U1( 0.f, 0.f, -1.f );
	VistaVector3D v3U2( v3Point2Transformed - v3Point1Transformed );
	VistaVector3D v3M = v3U2.Cross( v3U1 );
	float fM2 = v3M.Dot( v3M );
	VistaVector3D v3R = v3Point1Transformed.Cross( v3M ) / fM2;
	float fT = v3R.Dot( v3U1 );

	// we want to stay within the line defined by point 1 and 2
	fT = std::min( std::max( fT, .0f ), 1.f );

	VistaVector3D v3ClosestPoint( v3Point1Transformed + fT * v3U2 );

	VistaVector3D v3Return( m_ConeRef.TransformPositionFromFrame( v3ClosestPoint ) );

	return v3Return;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


