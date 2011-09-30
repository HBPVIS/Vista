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
// $Id: VistaGraphicsManager.cpp 21315 2011-05-16 13:47:39Z dr165799 $

#if defined(WIN32)
#pragma warning(disable: 4996)
#endif

// ============================================================================

#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSG.h>
#include <VistaKernel/GraphicsManager/VistaNodeBridge.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaLightNode.h>
#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
#include <VistaBase/VistaVectorMath.h>
#include <VistaTools/VistaProfiler.h>

#include <VistaKernel/EventManager/VistaGraphicsEvent.h>
#include <VistaKernel/EventManager/VistaEventObserver.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>
#include <VistaKernel/VistaKernelOut.h>


// IAR: remove FR-Observer, move to IntMgr
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>

#include <map>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

static bool GetCountFloats(VistaProfiler &LocalProfiler, const std::string &sSection, 
										   const std::string &sKey, 
										   const std::string &sIniFile,
										   std::vector<float> results, int iCount)
{
	std::list<std::string> list;
	LocalProfiler.GetTheProfileList(sSection, sKey, list, sIniFile);
	if (list.size() != iCount)
	{
		results.assign(iCount, 0.0f);
		return false;
	}
	else
	{
		std::list<std::string>::const_iterator listIter;
		int i=0;
		for(listIter = list.begin(); i<iCount; ++i,++listIter)
		results.push_back((float) atof(listIter->c_str()));
	}
	return true;
}


static bool GetCountFloats(VistaProfiler &LocalProfiler, const std::string &sSection, 
										   const std::string &sKey, 
										   const std::string &sIniFile,
										   VistaVector3D &storage)
{
	std::list<std::string> list;
	LocalProfiler.GetTheProfileList(sSection, sKey, list, sIniFile);
	if (list.size() != 3)
	{
		storage[Vista::X] = 0.0f;
		storage[Vista::Y] = 0.0f;
		storage[Vista::Z] = 0.0f;            
		return false;
	}
	else
	{
		std::list<std::string>::const_iterator listIter;
		int i=0;
		for(listIter = list.begin(); i<3; ++i,++listIter)
			storage[i] = ((float) atof(listIter->c_str()));
	}
	return true;
}


class VistaOverlayObserver : public VistaEventObserver
{
private:
	Vista2DText *m_pFrameRateText;
	VistaGraphicsManager *m_pGraMa;
	VistaDisplayManager *m_pDispMa;
	VistaEventManager    *m_pEvMa;
	std::string   m_strBuffer;
	double m_dLastCall;
protected:
public:
	VistaOverlayObserver(VistaEventManager *pEvMa, VistaDisplayManager *pDispMa, 
		VistaGraphicsManager *pGrMa,
		const std::string &strIniFName,
		const std::string &strViewportname);
	~VistaOverlayObserver();

	virtual void Notify(const VistaEvent *pEvent);

	bool GetEnabled() const 
	{ 
		return (m_pFrameRateText && (*m_pFrameRateText).GetEnabled()); 
	}

	void SetEnabled(bool bEnabled) 
	{ 
		if(m_pFrameRateText) 
			(*m_pFrameRateText).SetEnabled(bEnabled);
	}
};


VistaOverlayObserver::VistaOverlayObserver(VistaEventManager *pEvMa, 
											 VistaDisplayManager *pDispMa, 
											 VistaGraphicsManager *pGraMa,
											 const std::string &strIniFName,
											 const std::string &strViewportname)
{
	m_dLastCall = -1.0;
	m_pGraMa = pGraMa;
	m_pDispMa = pDispMa;
	m_pFrameRateText = m_pDispMa->Add2DText(strViewportname);
	VistaProfiler LocalProfiler;
	std::string sGraphicsSectionName;
	LocalProfiler.GetTheProfileString("SYSTEM", "GRAPHICSSECTION", "GRAPHICS", sGraphicsSectionName, strIniFName);

	float xpos = LocalProfiler.GetTheProfileFloat(sGraphicsSectionName, "FR_TEXT_X", 0.05f, strIniFName.c_str());
	float ypos = LocalProfiler.GetTheProfileFloat(sGraphicsSectionName, "FR_TEXT_Y", 0.9f, strIniFName.c_str());
	
	if(m_pFrameRateText)
		m_pFrameRateText->Init("", (float)xpos, (float)ypos);

	pEvMa->RegisterObserver(this, VistaSystemEvent::GetTypeId());

	m_strBuffer.resize(30);
	m_pEvMa = pEvMa;
}

VistaOverlayObserver::~VistaOverlayObserver()
{
	m_pEvMa->UnregisterObserver(this, VistaEvent::VET_ALL);
}

void VistaOverlayObserver::Notify(const VistaEvent *pEvent)
{
	//const VistaSystemEvent *pSysEv = static_cast<const VistaSystemEvent *>(pEvent);
	if(m_pFrameRateText && (pEvent->GetId() == VistaSystemEvent::VSE_PREGRAPHICS) && (*m_pFrameRateText).GetEnabled())
	{
			// do update ever 1.5th second ;)
			if((m_dLastCall == -1.0) || (pEvent->GetTime() - m_dLastCall > 1.5))
			{
				m_dLastCall = pEvent->GetTime();
				sprintf((char *)m_strBuffer.c_str(), "Framerate: %4.2f", m_pGraMa->GetFrameRate());
				m_pFrameRateText->SetText(m_strBuffer);
			}
	} 	
}


bool VistaGraphicsManager::RegisterEventTypes(VistaEventManager *pEventMgr)
{
	
	VistaEventManager::EVENTTYPE eTp = pEventMgr->RegisterEventType("VET_GRAPHICS");
	VistaGraphicsEvent::SetTypeId(eTp);


	for(int n = VistaGraphicsEvent::VGE_LIGHT_DIRECTION; n < VistaGraphicsEvent::VGE_UPPER_BOUND; ++n)
		pEventMgr->RegisterEventId(eTp, VistaGraphicsEvent::GetIdString(n));

	return true;
}


/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaGraphicsManager::VistaGraphicsManager(VistaEventManager *pEvMgr)
: VistaEventHandler(),
  m_pEvMgr(pEvMgr),
  m_pSceneGraph(NULL),
  m_pModelRoot(NULL),
  m_bShowInfo(false),
  m_pGraphicsBridge(NULL),
  m_pNodeBridge(NULL),
  m_pFrameRateText(NULL),
  m_pDispMgr(NULL)
{
}

VistaGraphicsManager::~VistaGraphicsManager()
{
	delete m_pSceneGraph;
	delete m_pFrameRateText;
	delete m_pGraphicsBridge;
	delete m_pNodeBridge;
}
// ============================================================================
// ============================================================================
bool VistaGraphicsManager::InitScene(IVistaNodeBridge* pNodeBridge, 
									  IVistaGraphicsBridge* pGraphicsBridge,
									  const std::string &strIniFName)
{
	int iNrLightAmbient		=0;
	int iNrLightDirectional	=0;
	int iNrLightPoint		=0;
	int iNrLightSpot		=0;

	m_strIniFile    = strIniFName;

	m_pSceneGraph     = new VistaSG;
	m_pGraphicsBridge = pGraphicsBridge;
	m_pNodeBridge     = pNodeBridge;
	
	if(m_pSceneGraph && m_pGraphicsBridge && m_pNodeBridge)
	{
		VistaProfiler LocalProfiler;
		std::string sGraphicsSectionName;
		LocalProfiler.GetTheProfileString("SYSTEM", "GRAPHICSSECTION", "GRAPHICS", sGraphicsSectionName, strIniFName);
		
		bool success = true;
		// init sub structures
		success &= m_pNodeBridge->Init(m_pSceneGraph);

		LocalProfiler.GetTheProfileString("SYSTEM", "GRAPHICSSECTION", "GRAPHICS", sGraphicsSectionName, strIniFName);

		/**
		 * @todo remove this construct
		 * if we need such a thing at all, rename to more speaking options like
		 * ROOT_X_AXIS = 1, 0, 0, 0 // baseaxis
		 * ROOT_Y_AXIS = 0, 1, 0, 0 // baseaxis
		 * ROOT_Z_AXIS = 0, 0, 1, 0 // baseaxis
		 * ROOT_W_AXIS = x, y, z, 1 // translation?
		 */

		success &= m_pSceneGraph->Init(m_pNodeBridge, pGraphicsBridge);
		//if everything has been all right so far -> create default scene setup
		if(success)
		{
			m_pModelRoot = m_pSceneGraph->GetRoot();
			if(m_pModelRoot)
			{
				//configure default lights
				list<string> lights;
				list<string>::iterator lightsIter;

				string sType;

				list<string> color;
				list<string>::iterator	colorIter;
				list<string> direction;
				list<string>::iterator	directionIter;
				list<string> position;
				list<string>::iterator	positionIter;
				list<string> attenuation;
				list<string>::iterator	attenuationIter;

				int i;
				float fArrTmp[3];
				VistaVector3D v3DTmp;

				float fLightIntensity;
				float fSpotCharacter;
				int   iSpotDistribution;

				LocalProfiler.GetTheProfileList(sGraphicsSectionName, "Lights", lights, strIniFName.c_str());
				if(lights.size() > 0)
				{
					for(lightsIter = lights.begin(); lightsIter != lights.end(); lightsIter++)
					{
						LocalProfiler.GetTheProfileString(*lightsIter, "Type", "", sType, strIniFName.c_str());
						if(! sType.empty())
						{
							vkernout << (*lightsIter) << " : " << sType << endl;
							// AmbientLight
							if(LocalProfiler.ValueEquals(sType, "AMBIENT"))
							{
								iNrLightAmbient++;
								
								VistaAmbientLight*	m_pAmbientLight = m_pSceneGraph->NewAmbientLight(m_pModelRoot);
								if( m_pAmbientLight == NULL )
								{
									vkernout << "Could not create ambient light [" << (*lightsIter)
										<< "] - there may already be an ambient light" << std::endl;
										continue;
								}
								m_pAmbientLight->SetName(*lightsIter);
						
								// AmbientColor
								LocalProfiler.GetTheProfileList(*lightsIter, "AmbientColor", color, strIniFName.c_str());
								if (color.size() != 3)
								{
									fArrTmp[0] = 0.0f;
									fArrTmp[1] = 0.0f;
									fArrTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,colorIter = color.begin(); i<3; ++i,++colorIter)
									fArrTmp[i] = (float) atof(colorIter->c_str());
								}
								m_pAmbientLight->SetAmbientColor(fArrTmp[0],fArrTmp[1],fArrTmp[2]);
								color.clear();

								//Intensity
								fLightIntensity = LocalProfiler.GetTheProfileFloat((*lightsIter).c_str(),"Intensity", 1.0f, strIniFName.c_str());
								m_pAmbientLight->SetIntensity(fLightIntensity);
							}
							else

							// DirectionalLight
							if(LocalProfiler.ValueEquals(sType, "DIRECTIONAL"))
							{
								iNrLightDirectional++;

								VistaDirectionalLight*	m_pDirectionalLight = m_pSceneGraph->NewDirectionalLight(m_pModelRoot);
								m_pDirectionalLight->SetName(*lightsIter);

								// AmbientColor
								LocalProfiler.GetTheProfileList(*lightsIter, "AmbientColor", color, strIniFName.c_str());
								if (color.size() != 3)
								{
									fArrTmp[0] = 0.0f;
									fArrTmp[1] = 0.0f;
									fArrTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,colorIter = color.begin(); i<3; ++i,++colorIter)
									fArrTmp[i] = (float) atof(colorIter->c_str());
								}
								m_pDirectionalLight->SetAmbientColor(fArrTmp[0],fArrTmp[1],fArrTmp[2]);
								color.clear();

								// DiffuseColor
								LocalProfiler.GetTheProfileList(*lightsIter, "DiffuseColor", color, strIniFName.c_str());
								if (color.size() != 3)
								{
									fArrTmp[0] = 0.0f;
									fArrTmp[1] = 0.0f;
									fArrTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,colorIter = color.begin(); i<3; ++i,++colorIter)
									fArrTmp[i] = (float) atof(colorIter->c_str());
								}
								m_pDirectionalLight->SetDiffuseColor(fArrTmp[0],fArrTmp[1],fArrTmp[2]);
								color.clear();

								// SpecularColor
								LocalProfiler.GetTheProfileList(*lightsIter, "SpecularColor", color, strIniFName.c_str());
								if (color.size() != 3)
								{
									fArrTmp[0] = 0.0f;
									fArrTmp[1] = 0.0f;
									fArrTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,colorIter = color.begin(); i<3; ++i,++colorIter)
									fArrTmp[i] = (float) atof(colorIter->c_str());
								}
								m_pDirectionalLight->SetSpecularColor(fArrTmp[0],fArrTmp[1],fArrTmp[2]);
								color.clear();

								// Direction
								LocalProfiler.GetTheProfileList(*lightsIter, "Direction", direction, strIniFName.c_str());
								if (direction.size() != 3)
								{
									v3DTmp[0] = 0.0f;
									v3DTmp[1] = 0.0f;
									v3DTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,directionIter = direction.begin(); i<3; ++i,++directionIter)
									v3DTmp[i] = (float) atof(directionIter->c_str());
								}
								m_pDirectionalLight->SetDirection(v3DTmp);
								direction.clear();

								//Intensity
								fLightIntensity = LocalProfiler.GetTheProfileFloat((*lightsIter).c_str(),"Intensity", 1.0f, strIniFName.c_str());
								m_pDirectionalLight->SetIntensity(fLightIntensity);
							}
							else

							// PointLight		
							if(LocalProfiler.ValueEquals(sType, "POINT"))
							{
								iNrLightPoint++;
								VistaPointLight*	m_pPointLight = m_pSceneGraph->NewPointLight(m_pModelRoot);
								m_pPointLight->SetName(*lightsIter);

								// AmbientColor
								LocalProfiler.GetTheProfileList(*lightsIter, "AmbientColor", color, strIniFName.c_str());
								if (color.size() != 3)
								{
									fArrTmp[0] = 0.0f;
									fArrTmp[1] = 0.0f;
									fArrTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,colorIter = color.begin(); i<3; ++i,++colorIter)
									fArrTmp[i] = (float) atof(colorIter->c_str());
								}
								m_pPointLight->SetAmbientColor(fArrTmp[0],fArrTmp[1],fArrTmp[2]);
								color.clear();

								// DiffuseColor
								LocalProfiler.GetTheProfileList(*lightsIter, "DiffuseColor", color, strIniFName.c_str());
								if (color.size() != 3)
								{
									fArrTmp[0] = 0.0f;
									fArrTmp[1] = 0.0f;
									fArrTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,colorIter = color.begin(); i<3; ++i,++colorIter)
									fArrTmp[i] = (float) atof(colorIter->c_str());
								}
								m_pPointLight->SetDiffuseColor(fArrTmp[0],fArrTmp[1],fArrTmp[2]);
								color.clear();

								// SpecularColor
								LocalProfiler.GetTheProfileList(*lightsIter, "SpecularColor", color, strIniFName.c_str());
								if (color.size() != 3)
								{
									fArrTmp[0] = 0.0f;
									fArrTmp[1] = 0.0f;
									fArrTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,colorIter = color.begin(); i<3; ++i,++colorIter)
									fArrTmp[i] = (float) atof(colorIter->c_str());
								}
								m_pPointLight->SetSpecularColor(fArrTmp[0],fArrTmp[1],fArrTmp[2]);
								color.clear();

								// Position
								LocalProfiler.GetTheProfileList(*lightsIter, "Position", position, strIniFName.c_str());
								if (position.size() != 3)
								{
									v3DTmp[0] = 0.0f;
									v3DTmp[1] = 0.0f;
									v3DTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,positionIter = position.begin(); i<3; ++i,++positionIter)
									v3DTmp[i] = (float) atof(positionIter->c_str());
								}
								m_pPointLight->SetPosition(v3DTmp);
								position.clear();

								// Attenuation
								LocalProfiler.GetTheProfileList(*lightsIter, "Attenuation", attenuation, strIniFName.c_str());
								if (attenuation.size() != 3)
								{
									v3DTmp[0] = 0.0f;
									v3DTmp[1] = 0.0f;
									v3DTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,attenuationIter = attenuation.begin(); i<3; ++i,++attenuationIter)
									v3DTmp[i] = (float) atof(attenuationIter->c_str());
								}
								m_pPointLight->SetAttenuation(v3DTmp);
								attenuation.clear();
								
								//Intensity
								fLightIntensity = LocalProfiler.GetTheProfileFloat((*lightsIter).c_str(),"Intensity", 1.0f, strIniFName.c_str());
								m_pPointLight->SetIntensity(fLightIntensity);
							}
							else
							
							// SpotLight
							if(LocalProfiler.ValueEquals(sType, "SPOT"))
							{
								iNrLightSpot++;

								VistaSpotLight*	m_pSpotLight = m_pSceneGraph->NewSpotLight(m_pModelRoot);
								m_pSpotLight->SetName(*lightsIter);

								// AmbientColor
								LocalProfiler.GetTheProfileList(*lightsIter, "AmbientColor", color, strIniFName.c_str());
								if (color.size() != 3)
								{
									fArrTmp[0] = 0.0f;
									fArrTmp[1] = 0.0f;
									fArrTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,colorIter = color.begin(); i<3; ++i,++colorIter)
									fArrTmp[i] = (float) atof(colorIter->c_str());
								}
								m_pSpotLight->SetAmbientColor(fArrTmp[0],fArrTmp[1],fArrTmp[2]);
								color.clear();

								// DiffuseColor
								LocalProfiler.GetTheProfileList(*lightsIter, "DiffuseColor", color, strIniFName.c_str());
								if (color.size() != 3)
								{
									fArrTmp[0] = 0.0f;
									fArrTmp[1] = 0.0f;
									fArrTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,colorIter = color.begin(); i<3; ++i,++colorIter)
									fArrTmp[i] = (float) atof(colorIter->c_str());
								}
								m_pSpotLight->SetDiffuseColor(fArrTmp[0],fArrTmp[1],fArrTmp[2]);
								color.clear();

								// SpecularColor
								LocalProfiler.GetTheProfileList(*lightsIter, "SpecularColor", color, strIniFName.c_str());
								if (color.size() != 3)
								{
									fArrTmp[0] = 0.0f;
									fArrTmp[1] = 0.0f;
									fArrTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,colorIter = color.begin(); i<3; ++i,++colorIter)
									fArrTmp[i] = (float) atof(colorIter->c_str());
								}
								m_pSpotLight->SetSpecularColor(fArrTmp[0],fArrTmp[1],fArrTmp[2]);
								color.clear();

								// Direction
								LocalProfiler.GetTheProfileList(*lightsIter, "Direction", direction, strIniFName.c_str());
								if (direction.size() != 3)
								{
									v3DTmp[0] = 0.0f;
									v3DTmp[1] = 0.0f;
									v3DTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,directionIter = direction.begin(); i<3; ++i,++directionIter)
									v3DTmp[i] = (float) atof(directionIter->c_str());
								}
								const VistaVector3D& d2 = v3DTmp;
								m_pSpotLight->SetDirection(d2);
								direction.clear();
								GetCountFloats(LocalProfiler, *lightsIter, "Position", strIniFName, v3DTmp);
								// Position
								LocalProfiler.GetTheProfileList(*lightsIter, "Position", position, strIniFName.c_str());
								if (position.size() != 3)
								{
									v3DTmp[0] = 0.0f;
									v3DTmp[1] = 0.0f;
									v3DTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,positionIter = position.begin(); i<3; ++i,++positionIter)
									v3DTmp[i] = (float) atof(positionIter->c_str());
								}
								const VistaVector3D& p2 = v3DTmp;
								m_pSpotLight->SetPosition(p2);
								position.clear();

								// Attenuation
								LocalProfiler.GetTheProfileList(*lightsIter, "Attenuation", attenuation, strIniFName.c_str());
								if (attenuation.size() != 3)
								{
									v3DTmp[0] = 0.0f;
									v3DTmp[1] = 0.0f;
									v3DTmp[2] = 0.0f;
								}
								else
								{
									for(i=0,attenuationIter = attenuation.begin(); i<3; ++i,++attenuationIter)
									v3DTmp[i] = (float) atof(attenuationIter->c_str());
								}
								const VistaVector3D& a2 = v3DTmp;
								m_pSpotLight->SetAttenuation(a2);
								attenuation.clear();

								//Intensity
								fLightIntensity = LocalProfiler.GetTheProfileFloat((*lightsIter).c_str(),"Intensity", 1.0f, strIniFName.c_str());
								m_pSpotLight->SetIntensity(fLightIntensity);

								// SpotCharacter (SpotCutOff)
								fSpotCharacter = LocalProfiler.GetTheProfileFloat(*lightsIter, "Character", 180.0, strIniFName.c_str());
								m_pSpotLight->SetSpotCharacter(fSpotCharacter);

								// Distribution (SpotExponent)
								iSpotDistribution = LocalProfiler.GetTheProfileInt(*lightsIter, "Distribution", 0, strIniFName.c_str());
								m_pSpotLight->SetSpotDistribution(iSpotDistribution);
							}
						}
					}
				
				return true;
				}
			}
		}
	}
	return false;
}

void VistaGraphicsManager::CreateDefaultLights()
{
	VistaVector3D v3DTmp;

					VistaDirectionalLight*	m_pDirectionalLight0 = m_pSceneGraph->NewDirectionalLight(m_pModelRoot);
					m_pDirectionalLight0->SetAmbientColor(0.1f, 0.1f, 0.1f);
					m_pDirectionalLight0->SetDiffuseColor(0.4f, 0.4f, 0.4f);
					m_pDirectionalLight0->SetSpecularColor(0.2f, 0.2f, 0.2f);
					m_pDirectionalLight0->SetName("DefaultDirectionalLight0");
					v3DTmp[0] = -1.0f;
					v3DTmp[1] =  1.0f;
					v3DTmp[2] =  0.0f;
					const VistaVector3D& d0 = v3DTmp;
					m_pDirectionalLight0->SetDirection(d0);

					VistaDirectionalLight*	m_pDirectionalLight1 = m_pSceneGraph->NewDirectionalLight(m_pModelRoot);
					m_pDirectionalLight0->SetAmbientColor(0.1f, 0.1f, 0.1f);
					m_pDirectionalLight0->SetDiffuseColor(0.4f, 0.4f, 0.4f);
					m_pDirectionalLight0->SetSpecularColor(0.2f, 0.2f, 0.2f);
					m_pDirectionalLight1->SetName("DefaultDirectionalLight1");
					v3DTmp[0] =  0.0f;
					v3DTmp[1] =  0.0f;
					v3DTmp[2] =  1.0f;
					const VistaVector3D& d1 = v3DTmp;
					m_pDirectionalLight1->SetDirection(d1);

					VistaDirectionalLight*	m_pDirectionalLight2 = m_pSceneGraph->NewDirectionalLight(m_pModelRoot);
					m_pDirectionalLight0->SetAmbientColor(0.1f, 0.1f, 0.1f);
					m_pDirectionalLight0->SetDiffuseColor(0.4f, 0.4f, 0.4f);
					m_pDirectionalLight0->SetSpecularColor(0.2f, 0.2f, 0.2f);
					m_pDirectionalLight2->SetName("DefaultDirectionalLight2");
					v3DTmp[0] =  1.0f;
					v3DTmp[1] = -1.0f;
					v3DTmp[2] = -1.0f;
					const VistaVector3D& d2 = v3DTmp;
					m_pDirectionalLight2->SetDirection(d2);

}


// ============================================================================
// ============================================================================

// ============================================================================
// ============================================================================
VistaSG* VistaGraphicsManager::GetSceneGraph() const 
{
	return m_pSceneGraph;
}
// ============================================================================
// ============================================================================
/*
VistaGroupNode* VistaGraphicsManager::GetModelRoot() const
{
	return m_pModelRoot;
}
*/
// ============================================================================
// ============================================================================
/*
VistaLightNode* VistaGraphicsManager::GetDefaultAmbientLight() const
{
	return m_pAmbientLight;
}
// ============================================================================
// ============================================================================
VistaLightNode* VistaGraphicsManager::GetDefaultSpotLight() const 
{
	return m_pSpotLight;
}
// ============================================================================
// ============================================================================
VistaLightNode* VistaGraphicsManager::GetDefaultPointLight() const
{
	return m_pPointLight;
}
*/
#if 0
// ============================================================================
// ============================================================================
bool VistaGraphicsManager::MoveScene(const VistaVector3D & rDir) 
{
	return m_pModelRoot->Translate(rDir);
}
// ============================================================================
// ============================================================================
bool VistaGraphicsManager::RotateScene(const VistaQuaternion & rQuat)
{
/*
	// rotiert im lokalen Koordinatensystem
	return m_pModelRoot->Rotate(pQuat);
*/

	// rotiert um lokalen Ursprung aber globale Achsen

	VistaTransformMatrix transform;
	m_pModelRoot->GetTransform(transform);

	VistaVector3D position = VistaVector3D(transform[0][3], transform[1][3], transform[2][3]);

	transform[0][3] = transform[1][3] = transform[2][3] = 0.0f;

	VistaTransformMatrix rotation = VistaTransformMatrix(rQuat);

	transform = rotation * transform ;

	transform[0][3] = position[0];
	transform[1][3] = position[1];
	transform[2][3] = position[2];

	m_pModelRoot->SetTransform(transform);

	return true;
}
// ============================================================================
// ============================================================================
bool VistaGraphicsManager::TransformScene(const VistaTransformMatrix &rTrans)
{
	return m_pModelRoot->SetTransform(rTrans);
}
// ============================================================================
// ============================================================================
bool VistaGraphicsManager::ResetScene()
{
	//UC
	return true;
}

// ============================================================================
// ============================================================================
VistaTransformMatrix	VistaGraphicsManager::GetRealToWorldTransform() const
{
	VistaVector3D vecRootPos;
	VistaQuaternion qRootOrient;

	m_pModelRoot->GetTranslation(vecRootPos);
	m_pModelRoot->GetRotation(qRootOrient);

	qRootOrient = qRootOrient.GetComplexConjugated();

	return VistaTransformMatrix(qRootOrient, qRootOrient.Rotate(-vecRootPos));
}

// ============================================================================
// ============================================================================
VistaTransformMatrix	VistaGraphicsManager::GetWorldToRealTransform() const
{
	VistaTransformMatrix transform;
	m_pModelRoot->GetTransform(transform);
	return transform;
}

// ============================================================================
// ============================================================================
void VistaGraphicsManager::TransformToWorld(const VistaVector3D &posIn, 
											 const VistaQuaternion &orientIn,
											 VistaVector3D &posOut, 
											 VistaQuaternion &orientOut) const
{
	VistaVector3D vecRootPos;
	VistaQuaternion qRootOrient;

	m_pModelRoot->GetTranslation(vecRootPos);
	m_pModelRoot->GetRotation(qRootOrient);

	// invert root node orientation (because we'll need it twice...)
	qRootOrient = qRootOrient.GetComplexConjugated();

	orientOut = qRootOrient * orientIn;
	posOut = qRootOrient.Rotate(posIn-vecRootPos);
}

// ============================================================================
// ============================================================================
void VistaGraphicsManager::TransformToReal(const VistaVector3D &posIn, 
											 const VistaQuaternion &orientIn,
											 VistaVector3D &posOut, 
											 VistaQuaternion &orientOut) const
{
	VistaVector3D vecRootPos;
	VistaQuaternion qRootOrient;

	m_pModelRoot->GetTranslation(vecRootPos);
	m_pModelRoot->GetRotation(qRootOrient);

	orientOut = qRootOrient * orientIn;
	posOut = qRootOrient.Rotate(posIn) + vecRootPos;
}
#endif

/*============================================================================*/
/*                                                                            */
/*  NAME      :   HandleEvent                                                 */
/*                                                                            */
/*============================================================================*/
void VistaGraphicsManager::HandleEvent(VistaEvent *pEvent)
{
}

bool VistaGraphicsManager::GetShowInfo() const
{
	return m_bShowInfo;
}

void VistaGraphicsManager::SetShowInfo(bool bInfo)
{
	m_bShowInfo = bInfo;
	if(m_bShowInfo)
	{
		if(!m_pFrameRateText && m_pDispMgr)
		{
			m_pFrameRateText = new VistaOverlayObserver(m_pEvMgr, 
				m_pDispMgr, 
				this,
				m_strIniFile,
				m_pDispMgr->GetViewportsConstRef().begin()->first);
		}
	}

	if(m_pFrameRateText)
	{
	   (static_cast<VistaOverlayObserver *>(m_pFrameRateText))->SetEnabled(m_bShowInfo);
	}
}

bool VistaGraphicsManager::GetCullingEnabled() const
{
	return m_pGraphicsBridge->GetCullingEnabled();
}

void VistaGraphicsManager::SetCullingEnabled(bool bCullingEnabled)
{
	return m_pGraphicsBridge->SetCullingEnabled(bCullingEnabled);
}

bool VistaGraphicsManager::GetOcclusionCullingEnabled() const
{
	return m_pGraphicsBridge->GetOcclusionCullingEnabled();
}

void VistaGraphicsManager::SetOcclusionCullingEnabled(bool bOclCullingEnabled)
{
	return m_pGraphicsBridge->SetOcclusionCullingEnabled(bOclCullingEnabled);
}

bool VistaGraphicsManager::GetBBoxDrawingEnabled() const
{
	return m_pGraphicsBridge->GetBBoxDrawingEnabled();
}

void VistaGraphicsManager::SetBBoxDrawingEnabled(bool bState)
{
	return m_pGraphicsBridge->SetBBoxDrawingEnabled(bState);
}


VistaDisplayManager *VistaGraphicsManager::GetDisplayManager() const
{
	return m_pDispMgr;
}

void VistaGraphicsManager::SetDisplayManager(VistaDisplayManager *pDispManager)
{
	m_pDispMgr = pDispManager;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetBackgroundColor                                          */
/*                                                                            */
/*============================================================================*/
VistaColorRGB VistaGraphicsManager::GetBackgroundColor() const
{
	return m_pGraphicsBridge->GetBackgroundColor();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetBackgroundColor                                          */
/*                                                                            */
/*============================================================================*/
void VistaGraphicsManager::SetBackgroundColor(const VistaColorRGB &color)
{
	m_pGraphicsBridge->SetBackgroundColor(color);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaGraphicsManager::Debug ( ostream & out ) const
{
	out << endl;
	out << " [ViGraMan] -------------------------------------------------------------" << endl;
	out << " [ViGraMan] INFO SCREEN " << endl;
	out << " [ViGraMan] VistaSceneGraph\t\t" << (m_pSceneGraph ? "constructed" : "construction FAILED.") << endl ;
	if(m_pSceneGraph) m_pSceneGraph->Debug(out);
//	out << " [ViGraMan] Default ambient light\t" << (m_pAmbientLight ? "initialized" : "FAILED init.") << endl ;
//	out << " [ViGraMan] Default spot light\t\t"    << (m_pSpotLight ? "initialized" : "FAILED init.") << endl ;
//	out << " [ViGraMan] Default point light\t\t"   << (m_pPointLight ? "initialized" : "FAILED init.") << endl ;
	out << " [ViGraMan]	Application is currently running at " << GetFrameRate() << " fps." << endl;
	out << " [ViGraMan] -------------------------------------------------------------" << endl;
}

IVistaNodeBridge* VistaGraphicsManager::GetNodeBridge() const
{
	return m_pNodeBridge;
}

IVistaGraphicsBridge* VistaGraphicsManager::GetGraphicsBridge() const
{
	return m_pGraphicsBridge;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
ostream & operator<< ( ostream & out, const VistaGraphicsManager & device )
{
	device.Debug ( out );
	return out;
}



