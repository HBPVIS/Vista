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

#ifndef _VISTASYSTEMCONFIGURATORS_H
#define _VISTASYSTEMCONFIGURATORS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include "VistaDriverPropertyConfigurator.h"
//#include "VistaSensorTransformConfigurator.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class VistaWindow;
class VistaConnection;
class VistaEventManager;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaWindowConfigurator : public VistaDriverPropertyConfigurator::IConfigurator
{
public:
	VistaWindowConfigurator(VistaDisplayManager *pSys);
	virtual ~VistaWindowConfigurator();


	// #########################################################################
	// PROPERTY ACCESSOR API
	// #########################################################################
	std::list<VistaWindow*> GetWindowsNameList() const;
	bool                     SetWindowsByNameList(const std::list<VistaWindow*> &liAttachWindows);


	// #########################################################################
	// ICONFIGURATOR API
	// #########################################################################
	bool GeneratePropertyListFromInifile(const std::string &strFile,
									const std::string &sRootSection,
									VistaPropertyList &oProps);

	// #########################################################################
	// IVistaReflectionable API
	// #########################################################################
	virtual std::string GetReflectionableType() const;
protected:
	virtual int AddToBaseTypeList(std::list<std::string> &rBtList) const;

private:
	VistaDisplayManager *m_pDispMgr;
	static std::string SsReflectionType;
};

class VISTAKERNELAPI VistaSensorMappingConfigurator : public VistaDriverPropertyConfigurator::IConfigurator
{
public:
	VistaSensorMappingConfigurator();
	virtual ~VistaSensorMappingConfigurator();

	// #########################################################################
	// PROPERTY ACCESSOR API
	// #########################################################################
	std::list<std::string> GetSensorNameList() const;
	bool SetSensorByNameList(const std::list<std::string> &liSensors);


	// #########################################################################
	// ICONFIGURATOR API
	// #########################################################################
	bool Configure(IVistaDeviceDriver *pDriver,
					   const VistaPropertyList &props);
	bool GeneratePropertyListFromInifile(const std::string &strFile,
									const std::string &sRootSection,
									VistaPropertyList &oProps);

	// #########################################################################
	// IVistaReflectionable API
	// #########################################################################
	virtual std::string GetReflectionableType() const;
protected:
	virtual int AddToBaseTypeList(std::list<std::string> &rBtList) const;
private:
	static std::string SsReflectionType;
};


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class VISTAKERNELAPI VistaGenericHistoryConfigurator : public VistaDriverPropertyConfigurator::IConfigurator
{
public:
	VistaGenericHistoryConfigurator();
	virtual ~VistaGenericHistoryConfigurator();

	// #########################################################################
	// ICONFIGURATOR API
	// #########################################################################
	bool Configure(IVistaDeviceDriver *pDriver,
					   const VistaPropertyList &props);
	bool GeneratePropertyListFromInifile(const std::string &strFile,
									const std::string &sRootSection,
									VistaPropertyList &oProps);

	// #########################################################################
	// IVistaReflectionable API
	// #########################################################################
	virtual std::string GetReflectionableType() const;
protected:
	virtual int AddToBaseTypeList(std::list<std::string> &rBtList) const;
private:
	static std::string SsReflectionType;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class VISTAKERNELAPI VistaConnectionConfigurator : public VistaDriverPropertyConfigurator::IConfigurator
{
public:
	VistaConnectionConfigurator();
	virtual ~VistaConnectionConfigurator();

	class VISTAKERNELAPI IConnectionFactoryMethod
	{
		friend class VistaConnectionConfigurator;
	public:
		virtual VistaConnection *CreateConnection(const VistaPropertyList &oProps) = 0;
	protected:
		void FailOnConnection(VistaConnection *pCon,
			const std::string &sReason) const;
		IConnectionFactoryMethod() {}
		virtual ~IConnectionFactoryMethod() {}
	};

	typedef std::map<std::string, IConnectionFactoryMethod*>::const_iterator const_iterator;
	typedef std::map<std::string, IConnectionFactoryMethod*>::iterator iterator;

	const_iterator begin() const;
	iterator       begin();
	const_iterator end() const;
	iterator       end();

	bool AddConFacMethod(const std::string &sKey, IConnectionFactoryMethod*);

	// #########################################################################
	// ICONFIGURATOR API
	// #########################################################################
	bool Configure(IVistaDeviceDriver *pDriver,
					   const VistaPropertyList &props);
	bool GeneratePropertyListFromInifile(const std::string &strFile,
									const std::string &sRootSection,
									VistaPropertyList &oProps);

	// #########################################################################
	// IVistaReflectionable API
	// #########################################################################
	REFL_INLINEIMP(VistaConnectionConfigurator,
				   VistaDriverPropertyConfigurator::IConfigurator)
public:
	std::map<std::string, IConnectionFactoryMethod*> m_mpFactories;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class VISTAKERNELAPI VistaDriverProtocolConfigurator : public VistaDriverPropertyConfigurator::IConfigurator
{
public:
	virtual bool Configure(IVistaDeviceDriver *pDriver,
		const VistaPropertyList &props);


	virtual bool GeneratePropertyListFromInifile(const std::string &strFile,
		const std::string &sRootSection,
		VistaPropertyList &);
	// #########################################################################
	// IVistaReflectionable API
	// #########################################################################
	REFL_INLINEIMP(VistaDriverProtocolConfigurator,
				   VistaDriverProtocolConfigurator::IConfigurator)
};

class VISTAKERNELAPI VistaDriverLoggingConfigurator : public VistaDriverPropertyConfigurator::IConfigurator
{
public:
	VistaDriverLoggingConfigurator();
	~VistaDriverLoggingConfigurator();

	// #########################################################################
	// ICONFIGURATOR API
	// #########################################################################

	virtual bool Configure(IVistaDeviceDriver *pDriver,
		const VistaPropertyList &props);


	virtual bool GeneratePropertyListFromInifile(const std::string &strFile,
		const std::string &sRootSection,
		VistaPropertyList &);
	// #########################################################################
	// IVistaReflectionable API
	// #########################################################################
	REFL_INLINEIMP(VistaDriverLoggingConfigurator,
				   VistaDriverPropertyConfigurator::IConfigurator)
private:
};

class VISTAKERNELAPI VistaDriverAttachOnlyConfigurator : public VistaDriverPropertyConfigurator::IConfigurator
{
public:
	VistaDriverAttachOnlyConfigurator();
	~VistaDriverAttachOnlyConfigurator();

	// #########################################################################
	// ICONFIGURATOR API
	// #########################################################################

	virtual bool Configure(IVistaDeviceDriver *pDriver,
		const VistaPropertyList &props);


	virtual bool GeneratePropertyListFromInifile(const std::string &strFile,
		const std::string &sRootSection,
		VistaPropertyList &);
	// #########################################################################
	// IVistaReflectionable API
	// #########################################################################
	REFL_INLINEIMP(VistaDriverAttachOnlyConfigurator,
				   VistaDriverPropertyConfigurator::IConfigurator)
private:
};

class VISTAKERNELAPI VistaDeviceIdentificationConfigurator : public VistaDriverPropertyConfigurator::IConfigurator
{
public:
	VistaDeviceIdentificationConfigurator();
	~VistaDeviceIdentificationConfigurator();

	// #########################################################################
	// ICONFIGURATOR API
	// #########################################################################

	virtual bool Configure(IVistaDeviceDriver *pDriver,
						   const VistaPropertyList &props);


	virtual bool GeneratePropertyListFromInifile(const std::string &strFile,
		const std::string &sRootSection,
		VistaPropertyList &);
	// #########################################################################
	// IVistaReflectionable API
	// #########################################################################
	REFL_INLINEIMP(VistaDriverAttachOnlyConfigurator,
				   VistaDriverPropertyConfigurator::IConfigurator)
private:
};


class VISTAKERNELAPI VistaDriverReferenceFrameConfigurator : public VistaDriverPropertyConfigurator::IConfigurator
{
public:
	VistaDriverReferenceFrameConfigurator();
	~VistaDriverReferenceFrameConfigurator();

	// #########################################################################
	// ICONFIGURATOR API
	// #########################################################################

	virtual bool Configure(IVistaDeviceDriver *pDriver,
		const VistaPropertyList &props);


	virtual bool GeneratePropertyListFromInifile(const std::string &strFile,
		const std::string &sRootSection,
		VistaPropertyList &);
	// #########################################################################
	// IVistaReflectionable API
	// #########################################################################
	REFL_INLINEIMP(VistaDriverReferenceFrameConfigurator,
				   VistaDriverPropertyConfigurator::IConfigurator)
private:
};


class VISTAKERNELAPI VistaDriverParameterConfigurator : public VistaDriverPropertyConfigurator::IConfigurator
{
public:
	VistaDriverParameterConfigurator();
	~VistaDriverParameterConfigurator();

	// #########################################################################
	// ICONFIGURATOR API
	// #########################################################################

	virtual bool Configure( IVistaDeviceDriver *pDriver,
							const VistaPropertyList &props);


	virtual bool GeneratePropertyListFromInifile(const std::string &strFile,
		const std::string &sRootSection,
		VistaPropertyList &);
	// #########################################################################
	// IVistaReflectionable API
	// #########################################################################
	REFL_INLINEIMP(VistaDriverReferenceFrameConfigurator,
				   VistaDriverPropertyConfigurator::IConfigurator)
private:
};



//// #############################################################################
//// SENSOR TRANSFORMS
//// #############################################################################
//
//class VISTAKERNELAPI VistaSensorFrameTransformConfigurator : public VistaSensorTransformConfigurator::IConfigurator
//{
//public:
//	VistaSensorFrameTransformConfigurator();
//	~VistaSensorFrameTransformConfigurator();
//
//	// #########################################################################
//	// ICONFIGURATOR API
//	// #########################################################################
//
//	virtual bool Configure(IVistaSensorTransform *pDriver,
//		const PropertyList &props);
//
//
//	virtual bool GeneratePropertyListFromInifile(const std::string &strFile,
//		const std::string &sRootSection,
//		PropertyList &,
//		const std::list<std::string> &liSearchpath = std::list<std::string>());
//	// #########################################################################
//	// IVistaReflectionable API
//	// #########################################################################
//	REFL_INLINEIMP(VistaSensorFrameTransformConfigurator,
//				   VistaSensorTransformConfigurator::IConfigurator)
//private:
//};
//
//class VISTAKERNELAPI VistaDisjunctTransformConfigurator : public VistaSensorTransformConfigurator::IConfigurator
//{
//public:
//	VistaDisjunctTransformConfigurator(VistaSensorTransformConfigurator *pConfigure);
//	~VistaDisjunctTransformConfigurator();
//
//	// #########################################################################
//	// ICONFIGURATOR API
//	// #########################################################################
//
//	virtual bool Configure(IVistaSensorTransform *pDriver,
//		const PropertyList &props);
//
//
//	virtual bool GeneratePropertyListFromInifile(const std::string &strFile,
//		const std::string &sRootSection,
//		PropertyList &,
//		const std::list<std::string> &liSearchpath = std::list<std::string>());
//	// #########################################################################
//	// IVistaReflectionable API
//	// #########################################################################
//	REFL_INLINEIMP(VistaSensorFrameTransformConfigurator,
//				   VistaSensorTransformConfigurator::IConfigurator)
//private:
//	IVistaSensorTransform *CreateSensorTransform(
//								const std::string &strType,
//								const PropertyList &oProps);
//
//	VistaSensorTransformConfigurator *m_pTransConfig;
//};
//
//// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//class VISTAKERNELAPI VistaNormalizeTransformConfigurator : public VistaSensorTransformConfigurator::IConfigurator
//{
//public:
//	VistaNormalizeTransformConfigurator(VistaDisplayManager *pDispMgr);
//	~VistaNormalizeTransformConfigurator();
//
//	// #########################################################################
//	// ICONFIGURATOR API
//	// #########################################################################
//
//	virtual bool Configure(IVistaSensorTransform *pDriver,
//		const PropertyList &props);
//
//
//	virtual bool GeneratePropertyListFromInifile(const std::string &strFile,
//		const std::string &sRootSection,
//		PropertyList &,
//		const std::list<std::string> &liSearchpath = std::list<std::string>());
//	// #########################################################################
//	// IVistaReflectionable API
//	// #########################################################################
//	REFL_INLINEIMP(VistaNormalizeTransformConfigurator,
//				   VistaSensorTransformConfigurator::IConfigurator)
//private:
//	VistaDisplayManager *m_pDispMgr;
//};
//
//class VISTAKERNELAPI Vista3DMouseTransformConfigurator : public VistaSensorTransformConfigurator::IConfigurator
//{
//public:
//	Vista3DMouseTransformConfigurator(VistaDisplayManager *pDispMgr);
//	~Vista3DMouseTransformConfigurator();
//
//	// #########################################################################
//	// ICONFIGURATOR API
//	// #########################################################################
//
//	virtual bool Configure(IVistaSensorTransform *pDriver,
//		const PropertyList &props);
//
//
//	virtual bool GeneratePropertyListFromInifile(const std::string &strFile,
//		const std::string &sRootSection,
//		PropertyList &,
//		const std::list<std::string> &liSearchpath = std::list<std::string>());
//	// #########################################################################
//	// IVistaReflectionable API
//	// #########################################################################
//	REFL_INLINEIMP(Vista3DMouseTransformConfigurator,
//				   VistaSensorTransformConfigurator::IConfigurator)
//private:
//	VistaDisplayManager *m_pDispMgr;
//};
//
//
//// ##############################################################################
//class VISTAKERNELAPI VistaGenericCommandMapTransformConfigurator : public VistaSensorTransformConfigurator::IConfigurator
//{
//public:
//	// #########################################################################
//	// ICONFIGURATOR API
//	// #########################################################################
//
//	virtual bool Configure(IVistaSensorTransform *pTrans,
//		const PropertyList &props);
//
//
//	virtual bool GeneratePropertyListFromInifile(const std::string &strFile,
//		const std::string &sRootSection,
//		PropertyList &oProps,
//		const std::list<std::string> &liSearchpath = std::list<std::string>());
//	// #########################################################################
//	// IVistaReflectionable API
//	// #########################################################################
//	REFL_INLINEIMP(VistaGenericCommandMapTransformConfigurator,
//				   VistaSensorTransformConfigurator::IConfigurator)
//private:
//};
//
//// ##############################################################################
//// ##############################################################################
//class VISTAKERNELAPI VistaPipedTransformConfig : public VistaSensorTransformConfigurator::IConfigurator
//{
//	public:
//	VistaPipedTransformConfig(VistaSensorTransformConfigurator *pConfigure);
//
//
//	IVistaSensorTransform *CreateSensorTransform(const std::string &strType,
//												const PropertyList &oProps);
//
//	bool Configure(IVistaSensorTransform *pTrans, const PropertyList &oProps);
//
//	bool GeneratePropertyListFromInifile( const std::string &strFile,
//									  const std::string &strRootSection,
//									  PropertyList &oStorage,
//									  const std::list<std::string> &liSearchPath);
//
//
//	REFL_INLINEIMP(VistaPipeTransformConfig,
//				   VistaSensorTransformConfigurator::IConfigurator);
//	private:
//	VistaSensorTransformConfigurator *m_pTransConfig;
//};
//
//
//
//// #####################################################################################
//
//class VISTAKERNELAPI VistaRefFrameTransformConfig : public VistaSensorTransformConfigurator::IConfigurator
//{
//	public:
//	VistaRefFrameTransformConfig(VistaDisplayManager *pSys);
//
//	bool Configure(IVistaSensorTransform *pTrans, const PropertyList &oProps);
//
//	bool GeneratePropertyListFromInifile( const std::string &strFile,
//									  const std::string &strRootSection,
//									  PropertyList &oStorage,
//									  const std::list<std::string> &liSearchPath);
//
//
//	REFL_INLINEIMP(VistaRefFrameTransformConfig,
//				   VistaSensorTransformConfigurator::IConfigurator);
//
//	private:
//	VistaDisplayManager *m_pSys;
//};
//
//// #####################################################################################
//class VISTAKERNELAPI VistaSlotValueMultiplexTransformConfig : public VistaSensorTransformConfigurator::IConfigurator
//{
//public:
//	VistaSlotValueMultiplexTransformConfig();
//
//	bool Configure(IVistaSensorTransform *pTrans, const PropertyList &oProps);
//
//	bool GeneratePropertyListFromInifile( const std::string &strFile,
//									  const std::string &strRootSection,
//									  PropertyList &oStorage,
//									  const std::list<std::string> &liSearchPath);
//
//
//	REFL_INLINEIMP(VistaSlotValueMultiplexTransformConfig,
//				   VistaSensorTransformConfigurator::IConfigurator);
//};
//
//// #####################################################################################
//
//class VISTAKERNELAPI VistaSensorFrameTransformCreationMethod : public VistaSensorTransformConfigurator::ITransformCreationMethod
//{
//public:
//	IVistaSensorTransform *CreateTransform();
//};
//
//class VISTAKERNELAPI VistaIdentityTransformCreationMethod : public VistaSensorTransformConfigurator::ITransformCreationMethod
//{
//public:
//	IVistaSensorTransform *CreateTransform();
//};
//
//class VISTAKERNELAPI VistaCoordIdentityTransformCreationMethod : public VistaSensorTransformConfigurator::ITransformCreationMethod
//{
//public:
//	IVistaSensorTransform *CreateTransform();
//};
//
//class VISTAKERNELAPI VistaCommandIdentityTransformCreationMethod : public VistaSensorTransformConfigurator::ITransformCreationMethod
//{
//public:
//	IVistaSensorTransform *CreateTransform();
//};
//
//class VISTAKERNELAPI VistaDisjunctTransformCreationMethod : public VistaSensorTransformConfigurator::ITransformCreationMethod
//{
//public:
//	IVistaSensorTransform *CreateTransform();
//};
//
//class VISTAKERNELAPI VistaMouseNormalizeCreationMethod : public VistaSensorTransformConfigurator::ITransformCreationMethod
//{
//public:
//	VistaMouseNormalizeCreationMethod(VistaEventManager *pEvMgr);
//	IVistaSensorTransform *CreateTransform();
//private:
//	VistaEventManager *m_pEvMgr;
//};
//
//class VISTAKERNELAPI Vista3DMouseTransformCreationMethod : public VistaSensorTransformConfigurator::ITransformCreationMethod
//{
//public:
//	Vista3DMouseTransformCreationMethod(VistaEventManager *pEvMgr);
//	IVistaSensorTransform *CreateTransform();
//private:
//	VistaEventManager *m_pEvMgr;
//};
//
//class VISTAKERNELAPI VistaGenericCommandMapTransformCreationMethod : public VistaSensorTransformConfigurator::ITransformCreationMethod
//{
//public:
//	IVistaSensorTransform *CreateTransform();
//};
//
//class VISTAKERNELAPI VistaPipedTransformCreate : public VistaSensorTransformConfigurator::ITransformCreationMethod
//{
//	public:
//	IVistaSensorTransform *CreateTransform();
//};
//
//
//class VISTAKERNELAPI VistaRefFrameTransformCreate : public VistaSensorTransformConfigurator::ITransformCreationMethod
//{
//	public:
//	IVistaSensorTransform *CreateTransform();
//};
//
//class VISTAKERNELAPI VistaSlotValueMultiplexTransformCreate : public VistaSensorTransformConfigurator::ITransformCreationMethod
//{
//	public:
//	IVistaSensorTransform *CreateTransform();
//};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEMCONFIGURATORS_H
