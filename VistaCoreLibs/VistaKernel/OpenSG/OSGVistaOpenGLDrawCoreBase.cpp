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
/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                  This file is automatically generated.                  **
 **                                                                         **
 **          Any changes made to this file WILL be lost when it is          **
 **           regenerated, which can become necessary at any time.          **
 **                                                                         **
 **     Do not change this file, changes should be done in the derived      **
 **     class VistaOpenGLDrawCore!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#define OSG_COMPILEVistaOpenGLDrawCoreINST


#include "OSGVistaOpenGLDrawCoreBase.h"
#include "OSGVistaOpenGLDrawCore.h"
#ifdef WIN32

// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4231)
#endif

#include <OpenSG/OSGConfig.h>

#ifdef WIN32
#pragma warning(pop)
#endif

#include <stdlib.h>
#include <stdio.h>


OSG_BEGIN_NAMESPACE

const OSG::BitVector VistaOpenGLDrawCoreBase::MTInfluenceMask = 
    (Inherited::MTInfluenceMask) | 
    (static_cast<BitVector>(0x0) << Inherited::NextFieldId); 

FieldContainerType VistaOpenGLDrawCoreBase::_type(
    "VistaOpenGLDrawCore",
    "MaterialDrawable",
    NULL,
    reinterpret_cast<PrototypeCreateF>(&VistaOpenGLDrawCoreBase::createEmpty),
    VistaOpenGLDrawCore::initMethod,
    NULL,
    0);

//OSG_FIELD_CONTAINER_DEF(VistaOpenGLDrawCoreBase, VistaOpenGLDrawCorePtr)

/*------------------------------ get -----------------------------------*/

FieldContainerType &VistaOpenGLDrawCoreBase::getType(void) 
{
    return _type; 
} 

const FieldContainerType &VistaOpenGLDrawCoreBase::getType(void) const 
{
    return _type;
} 


FieldContainerPtr VistaOpenGLDrawCoreBase::shallowCopy(void) const 
{ 
    VistaOpenGLDrawCorePtr returnValue; 

    newPtr(returnValue, dynamic_cast<const VistaOpenGLDrawCore *>(this)); 

    return returnValue; 
}

UInt32 VistaOpenGLDrawCoreBase::getContainerSize(void) const 
{ 
    return sizeof(VistaOpenGLDrawCore); 
}


#if !defined(OSG_FIXED_MFIELDSYNC)
void VistaOpenGLDrawCoreBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField)
{
    this->executeSyncImpl(static_cast<VistaOpenGLDrawCoreBase *>(&other),
                          whichField);
}
#else
void VistaOpenGLDrawCoreBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField,                                    const SyncInfo       &sInfo     )
{
    this->executeSyncImpl((VistaOpenGLDrawCoreBase *) &other, whichField, sInfo);
}
void VistaOpenGLDrawCoreBase::execBeginEdit(const BitVector &whichField, 
                                            UInt32     uiAspect,
                                            UInt32     uiContainerSize) 
{
    this->execBeginEditImpl(whichField, uiAspect, uiContainerSize);
}

void VistaOpenGLDrawCoreBase::onDestroyAspect(UInt32 uiId, UInt32 uiAspect)
{
    Inherited::onDestroyAspect(uiId, uiAspect);

}
#endif

/*------------------------- constructors ----------------------------------*/

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 383)
#endif

VistaOpenGLDrawCoreBase::VistaOpenGLDrawCoreBase(void) :
    Inherited() 
{
}

#ifdef OSG_WIN32_ICL
#pragma warning (default : 383)
#endif

VistaOpenGLDrawCoreBase::VistaOpenGLDrawCoreBase(const VistaOpenGLDrawCoreBase &source) :
    Inherited                 (source)
{
}

/*-------------------------- destructors ----------------------------------*/

VistaOpenGLDrawCoreBase::~VistaOpenGLDrawCoreBase(void)
{
}

/*------------------------------ access -----------------------------------*/

UInt32 VistaOpenGLDrawCoreBase::getBinSize(const BitVector &whichField)
{
    UInt32 returnValue = Inherited::getBinSize(whichField);


    return returnValue;
}

void VistaOpenGLDrawCoreBase::copyToBin(      BinaryDataHandler &pMem,
                                  const BitVector         &whichField)
{
    Inherited::copyToBin(pMem, whichField);


}

void VistaOpenGLDrawCoreBase::copyFromBin(      BinaryDataHandler &pMem,
                                    const BitVector    &whichField)
{
    Inherited::copyFromBin(pMem, whichField);


}

#if !defined(OSG_FIXED_MFIELDSYNC)
void VistaOpenGLDrawCoreBase::executeSyncImpl(      VistaOpenGLDrawCoreBase *pOther,
                                        const BitVector         &whichField)
{

    Inherited::executeSyncImpl(pOther, whichField);


}
#else
void VistaOpenGLDrawCoreBase::executeSyncImpl(      VistaOpenGLDrawCoreBase *pOther,
                                        const BitVector         &whichField,
                                        const SyncInfo          &sInfo      )
{

    Inherited::executeSyncImpl(pOther, whichField, sInfo);

}

void VistaOpenGLDrawCoreBase::execBeginEditImpl (const BitVector &whichField, 
                                                 UInt32     uiAspect,
                                                 UInt32     uiContainerSize)
{
    Inherited::execBeginEditImpl(whichField, uiAspect, uiContainerSize);

}
#endif

OSG_END_NAMESPACE

#include <OpenSG/OSGSFieldTypeDef.inl>
#include <OpenSG/OSGMFieldTypeDef.inl>

OSG_BEGIN_NAMESPACE

#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
DataType FieldDataTraits<VistaOpenGLDrawCorePtr>::_type("VistaOpenGLDrawCorePtr", "MaterialDrawablePtr");
#endif

OSG_DLLEXPORT_SFIELD_DEF1(VistaOpenGLDrawCorePtr, );
OSG_DLLEXPORT_MFIELD_DEF1(VistaOpenGLDrawCorePtr, );


/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id$";
    static Char8 cvsid_hpp       [] = OSGVistaOpenGLDrawCoreBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGVistaOpenGLDrawCoreBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGVistaOpenGLDrawCoreFIELDS_HEADER_CVSID;
}

OSG_END_NAMESPACE

