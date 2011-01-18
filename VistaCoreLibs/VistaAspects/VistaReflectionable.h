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

#ifndef _VISTAREFLECTIONABLE_H
#define _VISTAREFLECTIONABLE_H

#include "VistaAspectsConfig.h"

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaObserveable.h"
#include "VistaPropertyAwareable.h"
#include "VistaNameable.h"
#include "VistaAspectsUtils.h"

#include "VistaPropertyFunctor.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaPropertyFunctorRegistry;


/*============================================================================*/
/* LOCAL DECLARATIONS                                                         */
/*============================================================================*/

#define REFL_INLINEIMP(classname, superclass) \
	public:\
	std::string GetReflectionableType() const \
	{\
		return #classname;\
	}\
	protected: \
	int AddToBaseTypeList(std::list<std::string> &rBtList) const \
	{ \
	int nRet = superclass::AddToBaseTypeList(rBtList); \
	rBtList.push_back(#classname);\
	return ++nRet; \
	}

#define REFL_DECLARE \
	public: \
	std::string GetReflectionableType() const; \
	protected: \
	int AddToBaseTypeList(std::list<std::string> &rBtList) const;

#define REFL_IMPLEMENT(classname, superclass) \
	std::string classname::GetReflectionableType() const \
	{\
	return #classname;\
	}\
	\
	int classname::AddToBaseTypeList(std::list<std::string> &rBtList) const \
	{ \
		int nRet = superclass::AddToBaseTypeList(rBtList); \
		rBtList.push_back(#classname);\
		return ++nRet; \
	}

#define REFL_IMPLEMENT_FULL(classname, superclass) \
	namespace { std::string SsReflectionName( #classname ); } \
	std::string classname::GetReflectionableType() const \
	{\
	return #classname;\
	}\
	\
	int classname::AddToBaseTypeList(std::list<std::string> &rBtList) const \
	{ \
		int nRet = superclass::AddToBaseTypeList(rBtList); \
		rBtList.push_back(#classname);\
		return ++nRet; \
	}


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * A type implementing IVistaReflectionable is PropertyAwareable,
 * Observeable and Nameable in the ViSTA sense.
 */
class VISTAASPECTSAPI IVistaReflectionable : public IVistaPropertyAwareable,
							 public IVistaObserveable,
							 public IVistaNameable

{
public:
	virtual ~IVistaReflectionable();


	/**
	 * Override this method in subclasses. This is an absolute
	 * must. Use the macros REFL_INLINEIMP() in order to
	 * do so automatically. This method is called by GetBaseTypeList()
	 * The type name should not change during the application run.
	 * @return the application-wide unique type identifier for
	 *         the reflectionable specialization.
	 */
	virtual std::string GetReflectionableType() const;
	//#################################################
	// OBSERVEABLE-INTERFACE
	//#################################################


	//#################################################
	// NAMEABLE-INTERFACE
	//#################################################

	//virtual std::string GetNameForNameable() const;
	std::string GetNameForNameable() const;
	void   SetNameForNameable(const std::string &sName);


	/**
	 * Wrapper method for SetNameForNameable() (returns a bool
	 * instead of void)
	 * @return true iff sName changed the previous name
	 */

	//virtual bool   SetNameProp(const std::string &sName);
	bool   SetNameProp(const std::string &sName);

	//#################################################
	// PROPERTYAWAREABLE-INTERFACE
	//#################################################
	/**
	 * slightly changed behavior for reflectionables than
	 * for PropertyAwareables for SetProperty().
	 * @param oProp a reference to a property,
			  with a name set (oProp.GetName() != empty_string)
	 * @return PROPT_OK when the set functor
			   for the property returned true.
			   SetFunctors indicate true when
			   - the syntax was correct
			   - the new value replaced the old value
				 (meaning: was really a new value)
	 * @see IVistaPropertyGetFunctor()
	 */
	virtual int SetProperty(const VistaProperty &oProp);
	virtual int GetProperty(VistaProperty &);
	virtual VistaProperty GetPropertyByName(const std::string &sPropName);
	virtual bool SetPropertyByName(const std::string &sPropName,
								   const std::string &sPropValue);

	/**
	 * @return the number of changed properties from the PropertyList
			   in total.
	 */
	virtual int SetPropertiesByList(const VistaPropertyList &);
	virtual int GetPropertiesByList(VistaPropertyList &);

	virtual int GetPropertySymbolList(std::list<std::string> &rStorageList);
	virtual std::string GetPropertyDescription(const std::string &sPropName);

	/**
	 * Alias to GetBaseTypes(), but returns the list of parent
	 * type names on the stack (slightly more expensive, but convinient).
	 * @return a list of type names for the base types of this
			   IVistaReflectionable. The names are ordered to
			   indicate the inheritance chain from most abstract
			   to most specific.
	 * @see GetBaseTypes()
	 */
	std::list<std::string> GetBaseTypeList() const;

	/**
	 * Determines the base type names for this IVistaReflectionable
	 * as a sorted list from most abstract to most specific.
	 * The list is built up on the first call to this method and
	 * cached, so type names must not change during the application
	 * run. As the type names are defined by the subclass of the
	 * the reflectionable, it is important to keep them unique (hey,
	 * these are type names ;) and readable.
	 * @return the number of strings in rList
	 * @param rList a list to store the type names in
	 */
	int GetBaseTypes(std::list<std::string> &rList) const;

	enum
	{
		MSG_SETCHANGE = IVistaObserveable::MSG_LAST, /**< sent after SetPropsByList */
		MSG_NAMECHANGE,                              /**< sent upon SetNameForNameable() call */
		MSG_LAST
	};

	IVistaReflectionable(const IVistaReflectionable &);
	IVistaReflectionable &operator=(const IVistaReflectionable &);
protected:
	IVistaReflectionable();

	/**
	 *  This method <b>must</b> be overloaded by subclasses in order
	 * to build a propert hierarchy of base-type strings (i.e. strings
	 * that encode a unique class identifier). This Method will add its
	 * base type std::string as vanilla std::string to the rBtList.
	 * @return the number of entries in the list
	 * @param rBtList the list storage to keep track of base types
	 */
	virtual int AddToBaseTypeList(std::list<std::string> &rBtList) const;
private:
	void ValidateBaseTypeList();

	std::string            m_sName;
	std::list<std::string> m_liBaseTypeList;
};


template<class T> bool compAssignAndNotify(const T x, T &y, IVistaReflectionable &oObj, int nMsg)
{
	if( x != y )
	{
		y = x;
		oObj.Notify(nMsg);
		return true;
	}
	return false;
}


/**
* TVistaPropertyGet implements a templated getter functor for a
* IVistaReflectionable object. It should save you the job of writing an
* own functor in most of the cases.
* The meaning of the template arguments is as follows:
*
* R    return type of getter method
* C	   reflectionable for which to use/register the GetFunctor
*
*/
template<class R, class C, VistaProperty::ePropType nPropType = VistaProperty::PROPT_STRING> class TVistaPropertyGet
 : public IVistaPropertyGetFunctor
{
	public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which gets a single property from a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfm			pointer to the reflectionable's get method to be used
	*/
	TVistaPropertyGet(const std::string &sPropName,
					 const std::string &sClassType,
					 R (C::*pfm)() const,
					 const std::string &sDescription = "")
					 : IVistaPropertyGetFunctor(sPropName, sClassType, sDescription),
					   m_pfn(pfm)
					 {

					 }
	~TVistaPropertyGet()
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		R rTmp = (static_cast<const C&>(rObj).*m_pfn)();
		rProp.SetValue(VistaAspectsConversionStuff::ConvertToString(rTmp));
		rProp.SetPropertyType(nPropType);
		return !rProp.GetIsNilProperty();
	}

	protected:
	R (C::*m_pfn)() const;
	private:
	TVistaPropertyGet() {}
	TVistaPropertyGet(const std::string &sPropName,
					 const std::string &sClassType) {}
};


/**
* TVistaPropertyGet implements a templated getter functor for a
* IVistaReflectionable object. It should save you the job of writing an
* own functor in most of the cases.
* The meaning of the template arguments is as follows:
*
* R    return type of getter method
* Arg  argument type for conversion method
*      NOTE: This mithg be different from R due to some const / ref modifiers.
* C	   reflectionable for which to use/register the GetFunctor
*
*/
template<class R, class Arg, class C,
		VistaProperty::ePropType nType = VistaProperty::PROPT_STRING> class TVistaPropertyConvertAndGet
: public IVistaPropertyGetFunctor
{
	public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which gets a single property from a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfm			pointer to the reflectionable's get method to be used
	* @param	pCFn		pointer to the conversion function to be used
	*/
	TVistaPropertyConvertAndGet(const std::string &sPropName,
					 const std::string &sClassType,
					 R (C::*pfm)() const,
					 std::string (*pCFn)(Arg),
					 const std::string &sDescription = "")
					 : IVistaPropertyGetFunctor(sPropName, sClassType, sDescription),
					   m_pfn(pfm), m_pCFn(pCFn)
					 {
					 }
	~TVistaPropertyConvertAndGet()
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		R rTmp = (static_cast<const C&>(rObj).*m_pfn)();
		rProp.SetValue((*m_pCFn)(rTmp));
		rProp.SetPropertyType(nType);
		return !rProp.GetIsNilProperty();
	}


	protected:
	R (C::*m_pfn)() const;
	std::string (*m_pCFn)(Arg);
	private:
	TVistaPropertyConvertAndGet() {}
	TVistaPropertyConvertAndGet(const std::string &sPropName,
					 const std::string &sClassType) {}
};


/**
* TVistaPropertySet implements a templated SetFunctor for a
* IVistaReflectionable object. It should save you the job of writing an
* own functor in most of the cases.
* The meaning of the template arguments is as follows:
*
* - Arg  input type to reflectionable's setter method
* - CRes output type of conversion method std::string->input type
*        NOTE: This might be different from Arg due to some const
*            modifiers
* - C	   reflectionable for which to use/register the GetFunctor
*
* map functions like<br>
* bool C::SetMyFoo( const CMySpecialClass & );<br>
* with<br>
* TVistaPropertyGet<const CMySpecialClass &, CMySpecialClass, C>(
*  "MYFOO", SsReflectionName, &C::SetMyFoo, &CMyConversionFromString )<br>
* where MyConversionFromString is a static function, for example<br>
* CMySpecialClass CMyConversionFromString( const std::string &strMySpecialClassAsString )
*/
template <class Arg, class CRes,
		  class C> class TVistaPropertySet : public IVistaPropertySetFunctor
{
	public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which sets a single property of a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfn			pointer to the reflectionable's set method to be used
	* @param	pCFn		pointer to the conversion function to be used
	*/
	TVistaPropertySet(const std::string &sPropName,
					 const std::string &sClassType,
					 bool (C::*pfn)(Arg),
					 CRes (*pCFn)(const std::string &),
					 const std::string &sDescription = "")
					 : IVistaPropertySetFunctor(sPropName, sClassType, sDescription),
					   m_pfn(pfn), m_pCFn(pCFn)
					   {
					   }
	virtual ~TVistaPropertySet()
	{
	}

	virtual bool operator()(IVistaPropertyAwareable &rObj, const VistaProperty &rProp)
	{
		CRes rTmp = (*m_pCFn)(rProp.GetValue());
		// call setter, hope that CRes converts to Arg
		return (static_cast<C&>(rObj).*m_pfn)(rTmp);
	}


	protected:
	bool (C::*m_pfn)( Arg ); /**< the acual setter */
	CRes (*m_pCFn)(const std::string &); /**< conversion function */

	private:
	TVistaPropertySet() {}
	TVistaPropertySet(const std::string &sPropName,
					 const std::string &sClassType) {}
};


template<class C> class VistaPropertyListGetFunctor : public IVistaPropertyGetFunctor
{
public:
	VistaPropertyListGetFunctor(const std::string &sPropName,
							 const std::string &sClassName,
							 VistaPropertyList (C::*pfn)() const,
							 const std::string &sDescription = "")
		: IVistaPropertyGetFunctor(sPropName, sClassName, sDescription),
		 m_pfn(pfn)
	{
	}

	~VistaPropertyListGetFunctor()
	{
	}

		virtual bool operator()(const IVistaPropertyAwareable &rObj,
								VistaProperty &rProp) const
		{
			VistaPropertyList rProps = (static_cast<const C&>(rObj).*m_pfn)();
			rProp.SetPropertyListValue(rProps);
			rProp.SetPropertyType(VistaProperty::PROPT_PROPERTYLIST);
			return true;
		}


protected:
private:
	VistaPropertyList (C::*m_pfn)() const;
};

template<class C> class VistaPropertyListSetFunctor : public IVistaPropertySetFunctor
{
public:
	VistaPropertyListSetFunctor(const std::string &sName,
							 const std::string &sClassName,
							 bool (C::*pfn)(const VistaPropertyList &),
							 const std::string &sDescription = "")
		: IVistaPropertySetFunctor(sName, sClassName, sDescription),
		m_pfn(pfn)
	{
	}

	~VistaPropertyListSetFunctor()
	{
	}

	virtual bool operator()(IVistaPropertyAwareable &rObj, const VistaProperty &rProp)
	{
		return (static_cast<C&>(rObj).*m_pfn)(rProp.GetPropertyListValue());
	}

protected:
private:
	bool (C::*m_pfn)(const VistaPropertyList &);
};

template<class C> class TVistaPublicStringPropertyGet : public IVistaPropertyGetFunctor
{
	public:
	TVistaPublicStringPropertyGet(const std::string &sPropName,
					 const std::string &sClassType,
					 std::string (C::*sProp),
					 const std::string &sDescription = "")
					 : IVistaPropertyGetFunctor(sPropName, sClassType, sDescription),
					  m_sProp(sProp)
					 {

					 }

	~TVistaPublicStringPropertyGet()
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		rProp.SetValue(static_cast<const C&>(rObj).*m_sProp);
		rProp.SetPropertyType(VistaProperty::PROPT_STRING);
		return !rProp.GetIsNilProperty();
	}


	protected:
		std::string (C::*m_sProp);

	private:
	TVistaPublicStringPropertyGet() {}
	TVistaPublicStringPropertyGet(const std::string &sPropName,
					 const std::string &sClassType) {}
};


template <class C> class TVistaPublicStringPropertySet : public IVistaPropertySetFunctor
{
	public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which sets a single property of a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfn			pointer to the reflectionable's set method to be used
	* @param	pCFn		pointer to the conversion function to be used
	*/
	TVistaPublicStringPropertySet(const std::string &sPropName,
					 const std::string &sClassType,
					 std::string (C::*sProp),
					 const std::string &sDescription = "")
					 : IVistaPropertySetFunctor(sPropName, sClassType, sDescription),
					 m_sProp(sProp)
					   {
					   }
	virtual ~TVistaPublicStringPropertySet()
	{
	}

	virtual bool operator()(IVistaPropertyAwareable &rObj, const VistaProperty &rProp)
	{
		static_cast<C&>(rObj).*m_sProp = rProp.GetValue();
		return true;
	}


	protected:
		std::string (C::*m_sProp);

	private:
	TVistaPublicStringPropertySet() {}
	TVistaPublicStringPropertySet(const std::string &sPropName,
					 const std::string &sClassType) {}
};

/**
 * C - base type
 * A - ref type
 * usage, e.g. : bool Get(float &, float &) const;
 * -> TVistaProperty2RefGet<float, TYPE>("name", "type", &type::Get)
 */
template<class A, class C, VistaProperty::ePropType nPropSubType = VistaProperty::PROPT_DOUBLE>
	class TVistaProperty2RefGet : public IVistaPropertyGetFunctor
{
	public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which gets a single property from a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfm			pointer to the reflectionable's get method to be used
	*/
	TVistaProperty2RefGet(const std::string &sPropName,
					 const std::string &sClassType,
					 bool (C::*pfm)(A &, A &) const,
					 const std::string &sDescription = "")
					 : IVistaPropertyGetFunctor(sPropName, sClassType, sDescription),
					   m_pfn(pfm)
					 {

					 }
	~TVistaProperty2RefGet()
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		A a,b;
		if((static_cast<const C&>(rObj).*m_pfn)(a,b))
		{
			std::string sA, sB;
			sA = VistaAspectsConversionStuff::ConvertToString(a);
			sB = VistaAspectsConversionStuff::ConvertToString(b);

			rProp.SetValue(sA + std::string(", ") + sB);
			rProp.SetPropertyType(VistaProperty::PROPT_LIST);
			rProp.SetPropertyListSubType(nPropSubType);
		}

		return !rProp.GetIsNilProperty();
	}

	protected:
	bool (C::*m_pfn)(A &, A &) const;
	private:
	TVistaProperty2RefGet() {}
	TVistaProperty2RefGet(const std::string &sPropName,
					 const std::string &sClassType) {}
};


template <class Arg,
		  class C> class TVistaProperty2ValSet : public IVistaPropertySetFunctor
{
	public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which sets a single property of a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfn			pointer to the reflectionable's set method to be used
	* @param	pCFn		pointer to the conversion function to be used
	*/
	TVistaProperty2ValSet(const std::string &sPropName,
					 const std::string &sClassType,
					 bool (C::*pfn)(Arg, Arg),
					 bool (*pCFn)(const std::string &, Arg &, Arg & ),
					 const std::string &sDescription = "")
					 : IVistaPropertySetFunctor(sPropName, sClassType, sDescription),
					   m_pfn(pfn), m_pCFn(pCFn)
					   {
					   }
	virtual ~TVistaProperty2ValSet()
	{
	}

	virtual bool operator()(IVistaPropertyAwareable &rObj, const VistaProperty &rProp)
	{
		Arg a, b;
		if((*m_pCFn)(rProp.GetValue(), a,b))
		{
			// call setter, hope that CRes converts to Arg
			return (static_cast<C&>(rObj).*m_pfn)(a,b);
		}
		return false;
	}


	protected:
	bool (C::*m_pfn)( Arg, Arg ); /**< the acual setter */
	bool (*m_pCFn)(const std::string &, Arg &, Arg &); /**< conversion function */

	private:
	TVistaProperty2ValSet() {}
	TVistaProperty2ValSet(const std::string &sPropName,
					 const std::string &sClassType) {}
};

/**
 * C - base type
 * A - ref type
 * usage, e.g. : bool Get(float &, float &) const;
 * -> TVistaProperty2RefGet<float, TYPE>("name", "type", &type::Get)
 */
template<class A, class C, VistaProperty::ePropType nSubType = VistaProperty::PROPT_DOUBLE>
		 class TVistaProperty3RefGet : public IVistaPropertyGetFunctor
{
	public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which gets a single property from a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfm			pointer to the reflectionable's get method to be used
	*/
	TVistaProperty3RefGet(const std::string &sPropName,
					 const std::string &sClassType,
					 bool (C::*pfm)(A&, A&, A&) const,
					 const std::string &sDescription = "")
					 : IVistaPropertyGetFunctor(sPropName, sClassType, sDescription),
					   m_pfn(pfm)
					 {

					 }
	~TVistaProperty3RefGet()
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		A a,b,c;
		if((static_cast<const C&>(rObj).*m_pfn)(a,b,c))
		{
			std::string sA, sB, sC;
			sA = VistaAspectsConversionStuff::ConvertToString(a);
			sB = VistaAspectsConversionStuff::ConvertToString(b);
			sC = VistaAspectsConversionStuff::ConvertToString(c);

			rProp.SetValue(sA + std::string(", ") + sB + std::string(", ") + sC);
			rProp.SetPropertyType(VistaProperty::PROPT_LIST);
			rProp.SetPropertyListSubType(nSubType);
		}

		return !rProp.GetIsNilProperty();
	}

	protected:
	bool (C::*m_pfn)(A&, A&, A&) const;
	private:
	TVistaProperty3RefGet() {}
	TVistaProperty3RefGet(const std::string &sPropName,
					 const std::string &sClassType) {}
};

template <class Arg,
		  class C> class TVistaProperty3ValSet : public IVistaPropertySetFunctor
{
	public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which sets a single property of a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfn			pointer to the reflectionable's set method to be used
	* @param	pCFn		pointer to the conversion function to be used
	*/
	TVistaProperty3ValSet(const std::string &sPropName,
					 const std::string &sClassType,
					 bool (C::*pfn)(Arg, Arg, Arg),
					 bool (*pCFn)(const std::string &, Arg &, Arg &, Arg & ),
					 const std::string &sDescription = "")
					 : IVistaPropertySetFunctor(sPropName, sClassType, sDescription),
					   m_pfn(pfn), m_pCFn(pCFn)
					   {
					   }
	virtual ~TVistaProperty3ValSet()
	{
	}

	virtual bool operator()(IVistaPropertyAwareable &rObj, const VistaProperty &rProp)
	{
		Arg a, b, c;
		if((*m_pCFn)(rProp.GetValue(), a,b,c))
		{
			// call setter, hope that CRes converts to Arg
			return (static_cast<C&>(rObj).*m_pfn)(a,b,c);
		}
		return false;
	}


	protected:
	bool (C::*m_pfn)( Arg, Arg, Arg ); /**< the acual setter */
	bool (*m_pCFn)(const std::string &, Arg &, Arg &, Arg &); /**< conversion function */

	private:
	TVistaProperty3ValSet() {}
	TVistaProperty3ValSet(const std::string &sPropName,
					 const std::string &sClassType) {}
};


/**
 * C - base type
 * A - ref type
 * usage, e.g. : bool Get(float &, float &) const;
 * -> TVistaProperty2RefGet<float, TYPE>("name", "type", &type::Get)
 */
template<class A, class C, VistaProperty::ePropType nSubType = VistaProperty::PROPT_DOUBLE>
		 class TVistaProperty4RefGet : public IVistaPropertyGetFunctor
{
	public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which gets a single property from a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfm			pointer to the reflectionable's get method to be used
	*/
	TVistaProperty4RefGet(const std::string &sPropName,
					 const std::string &sClassType,
					 bool (C::*pfm)(A&, A&, A&, A&) const,
					 const std::string &sDescription = "")
					 : IVistaPropertyGetFunctor(sPropName, sClassType, sDescription),
					   m_pfn(pfm)
					 {

					 }
	~TVistaProperty4RefGet()
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		A a,b,c,d;
		if((static_cast<const C&>(rObj).*m_pfn)(a,b,c,d))
		{
			std::string sA, sB, sC, sD;
			sA = VistaAspectsConversionStuff::ConvertToString(a);
			sB = VistaAspectsConversionStuff::ConvertToString(b);
			sC = VistaAspectsConversionStuff::ConvertToString(c);
			sD = VistaAspectsConversionStuff::ConvertToString(d);

			rProp.SetValue(sA + std::string(", ") + sB + std::string(", ") + sC + std::string(", ") + sD);
			rProp.SetPropertyType(VistaProperty::PROPT_LIST);
			rProp.SetPropertyListSubType(nSubType);
		}

		return !rProp.GetIsNilProperty();
	}

	protected:
	bool (C::*m_pfn)(A&, A&, A&, A&) const;
	private:
	TVistaProperty4RefGet() {}
	TVistaProperty4RefGet(const std::string &sPropName,
					 const std::string &sClassType) {}
};

template <class Arg,
		  class C> class TVistaProperty4ValSet : public IVistaPropertySetFunctor
{
	public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which sets a single property of a IVistaReflectionable
	*
	* @param	sPropName	std::string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfn			pointer to the reflectionable's set method to be used
	* @param	pCFn		pointer to the conversion function to be used
	*/
	TVistaProperty4ValSet(const std::string &sPropName,
					 const std::string &sClassType,
					 bool (C::*pfn)(Arg, Arg, Arg, Arg),
					 bool (*pCFn)(const std::string &, Arg &, Arg &, Arg &, Arg & ),
					 const std::string &sDescription = "")
					 : IVistaPropertySetFunctor(sPropName, sClassType, sDescription),
					   m_pfn(pfn), m_pCFn(pCFn)
					   {
					   }
	virtual ~TVistaProperty4ValSet()
	{
	}

	virtual bool operator()(IVistaPropertyAwareable &rObj, const VistaProperty &rProp)
	{
		Arg a, b, c, d;
		if((*m_pCFn)(rProp.GetValue(), a,b,c,d))
		{
			// call setter, hope that CRes converts to Arg
			return (static_cast<C&>(rObj).*m_pfn)(a,b,c,d);
		}
		return false;
	}


	protected:
	bool (C::*m_pfn)( Arg, Arg, Arg, Arg ); /**< the acual setter */
	bool (*m_pCFn)(const std::string &, Arg &, Arg &, Arg &, Arg &); /**< conversion function */

	private:
	TVistaProperty4ValSet() {}
	TVistaProperty4ValSet(const std::string &sPropName,
					 const std::string &sClassType) {}
};

/**
 * VistaGetArrayPropertyFunctor retrieves a named property which contains n floats
 * given in array format i.e. the getter has the signature
 * void GetFloats(float f[n]) const;
 * This can be used to retrieve e.g. a coordinate vector, a rotation, or a rgb tuple.
 *
 * CRefl		Reflectionable type
 * CArrayType	type of array elements
 * DIM			Number of floats to retrieve
 *
 */
template<class CRefl, class CArrayType, int DIM=3>
class VistaGetArrayPropertyFunctor : public IVistaPropertyGetFunctor
{
public:
	/**
	 * @param strPropertyName		name of the property to get
	 * @param strClassName			name of the reflectionable type
	 * @param getter				pointer to the reflectionable's getter method
	 */
	VistaGetArrayPropertyFunctor(	const std::string& strPropertyName,
									const std::string& strClassType,
									void (CRefl::*getter)(CArrayType *) const,
									const std::string &sDescription = "")
		:	IVistaPropertyGetFunctor(strPropertyName, strClassType, sDescription),
			m_pGetterFunc(getter)
	{}
	virtual ~VistaGetArrayPropertyFunctor(){}

	/**
	 * get property rProp from the given reflectionable rRefl
	 */
	virtual bool operator()(const IVistaPropertyAwareable& rRefl, VistaProperty &rProp) const
	{
		CArrayType f[DIM];
		(static_cast<const CRefl&>(rRefl).*m_pGetterFunc)(f);
		std::list<CArrayType> liVals;
		for(int i=0; i<DIM; ++i)
			liVals.push_back(f[i]);
		rProp.SetValue(VistaAspectsConversionStuff::ConvertToString(liVals));
		rProp.SetPropertyType(VistaProperty::PROPT_LIST);
		return !rProp.GetIsNilProperty();
	}
protected:
private:
	void (CRefl::*m_pGetterFunc)(CArrayType *f) const;

	VistaGetArrayPropertyFunctor() {};
};


/**
 * VistaSetArrayPropertyFunctor writes a named property of n floats
 * given in array format i.e. the setter has the signature
 * void SetFloats(float f[n]);
 * This can be used to write e.g. a coordinate vector, a rotation, or a rgb tuple.
 *
 * CRefl		Reflectionable type
 * CArrayType	type of array elements
 * DIM			Number of floats to retrieve
 *
 */
template<class CRefl, class CArrayType, int DIM=3>
class VistaSetArrayPropertyFunctor : public IVistaPropertySetFunctor
{
public:
	/**
	 * @param strPropertyName		name of the property to set
	 * @param strClassName			name of the reflectionable type
	 * @param setter				pointer to the reflectionable's setter method
	 * @param convert				pointer to the conversion method (std::string -> std::list<CArrayType>)
	 */
	VistaSetArrayPropertyFunctor(	const std::string& strPropertyName,
									const std::string& strClassName,
									bool (CRefl::*setter)(CArrayType*),
									std::list<CArrayType>(*convert)(const std::string&),
									const std::string &sDescription = "")
		:	IVistaPropertySetFunctor(strPropertyName, strClassName, sDescription),
			m_pSetterFunc(setter),
			m_pConversion(convert)
	{}

	virtual ~VistaSetArrayPropertyFunctor(){}

	/**
	 * set property rProp to the given reflectionable rRefl
	 */
	virtual bool operator()(IVistaPropertyAwareable& rRefl, const VistaProperty &rProp)
	{
		std::list<CArrayType> liVals = (*m_pConversion)(rProp.GetValue());
		if(liVals.size() != DIM)
			return false;
		CArrayType f[DIM];
		typename std::list<CArrayType>::iterator itCurVal = liVals.begin();
		for(int i=0; i<DIM; ++i, ++itCurVal)
			f[i] = *itCurVal;
		return (static_cast<CRefl&>(rRefl).*m_pSetterFunc)(f);
	}
protected:
private:
	bool (CRefl::*m_pSetterFunc)(CArrayType *f);
	std::list<CArrayType>(*m_pConversion)(const std::string&);

	VistaSetArrayPropertyFunctor(){}
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAREFLECTIONABLE_H

