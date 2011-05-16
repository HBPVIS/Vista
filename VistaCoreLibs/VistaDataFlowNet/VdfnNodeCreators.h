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

#ifndef _VDFNNODECREATORS_H
#define _VDFNNODECREATORS_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>
#include "VdfnSerializer.h"

#include "VdfnNodeFactory.h"

#include "VdfnConstantValueNode.h"
#include "VdfnChangeDetectNode.h"
#include "VdfnAggregateNode.h"
#include "VdfnVectorDecomposeNode.h"
#include "VdfnCounterNode.h"
#include "VdfnModuloCounterNode.h"
#include "VdfnThresholdNode.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*
 * this file is meant to contain node creators. In the DataFlowNet, node
 * creation is separated from the nodes on purpose most of the time.
 * Different applications may need to have different means of creating nodes.
 * All node creators expect a proplist with one sub-list, called 'params'.
 * This is in term evaluated further.
 */

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverMap;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * creates instances of type VdfnShallowNode.
 * reads in oParams for
 * - inports: pairs <name, value> of inports to create and assign
              to the shallow node. needs a registration of port access structures
              in the VdfnPortFactory in order to work properly.
 * - outports: pairs of <name,value> of outports to create and assign
 *
 * inports and outports are optional arguments
 */
class VISTADFNAPI VdfnShallowNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
    /**
     * inports: subtree of name=type lists
     * outports: subtree of name=type lists
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

/**
 * creates a VdfnTimerNode.
 */
class VISTADFNAPI VdfnTimerNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
    /**
     * single argument:
     * - reset_on_activate: type bool, controls whether the timer will be reset
                            on a call to activate on the node.
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

/**
 * creates VdfnTickTimerNode
 */
class VdfnTickTimerNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
    /**
     * no arguments
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

/**
 * creates VdfnUpdateThresholdNode
 */
class VdfnUpdateThresholdNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

/**
 * creates VdfnThresholdNode
 */

template<class T>
class VdfnThresholdNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
	 /**
     * arguments
     * - threshold: [number,optional] - the name of the driver that contains the sensor of desire
     * - compare_absolute_value: [bool,optional] - sets wether or not the absolute should be tested
     * - mode: [string,optional] - sets how values below the threshold are handles. options are:
	 *                             block | output_zero | output_threshold | output_last_value
     */
    virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const
	{
		double dThreshold = 0.0;
		bool bUseAbsoluteValue = false;
		int iMode = VdfnThresholdNode<T>::BTHM_BLOCK;
		const VistaPropertyList &subs 
				= oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

		if( subs.HasProperty("threshold") )
			dThreshold = subs.GetDoubleValue( "threshold" );

		if( subs.HasProperty( "compare_absolute_value" ) )
			bUseAbsoluteValue = subs.GetBoolValue( "compare_absolute_value" );

		
		if( subs.HasProperty( "mode" ) )
		{
			std::string sModeName = subs.GetStringValue( "mode" );
			if( sModeName == "block" )
				iMode = VdfnThresholdNode<T>::BTHM_BLOCK;
			else if( sModeName == "output_zero" )
				iMode = VdfnThresholdNode<T>::BTHM_OUTPUT_ZERO;
			else if( sModeName == "output_threshold" )
				iMode = VdfnThresholdNode<T>::BTHM_OUTPUT_THRESHOLD;
			else if( sModeName == "output_last_value" )
				iMode = VdfnThresholdNode<T>::BTHM_OUTPUT_LAST_VALID_VALUE;
		}


		return new VdfnThresholdNode<T>( static_cast<T>(dThreshold), bUseAbsoluteValue, iMode );
	}
};

/**
 * creates VdfnGetTransformNode, needs a pointer to VdfnObjectRegistry to pass to the node in
 * case the target object is not found during a call to CreateNode()
 */
class VISTADFNAPI VdfnGetTransformNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
    /**
     * the object registry must outlive this node creator.
     * @param pReg non NULL and persistent pointer to the object registry
     */
    VdfnGetTransformNodeCreate(VdfnObjectRegistry *pReg);

    /**
     * arguments
     * - object: [string] the name of an object transform (case-sensitive) to lookup in the
                 object registry
     * - mode: [string] RELATIVE evaluates local transform, WORLD evaluates global transforms
               default is RELATIVE (case-insensitive)
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;

private:
    VdfnObjectRegistry *m_pReg;
};

/**
 * creates an VdfnSetTransformNode, needs a pointer to VdfnObjectRegistry to pass to the node in
 * case the target object is not found during a call to CreateNode()
 */
class VISTADFNAPI VdfnSetTransformNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
    /**
     * the object registry must outlive this node creator.
     * @param pReg non NULL and persistent pointer to the object registry
     */
    VdfnSetTransformNodeCreate(VdfnObjectRegistry *pReg);

    /**
     * arguments
     * - object: [string] the name of an object transform (case-sensitive) to lookup in the
                 object registry
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
private:
    VdfnObjectRegistry *m_pReg;
};

/**
 * creates an VdfnApplyTransformNode, needs a pointer to VdfnObjectRegistry to pass to the node in
 * case the target object is not found during a call to CreateNode()
 */
class VISTADFNAPI VdfnApplyTransformNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
    /**
     * the object registry must outlive this node creator.
     * @param pReg non NULL and persistent pointer to the object registry
     */
    VdfnApplyTransformNodeCreate(VdfnObjectRegistry *pReg);

    /**
     * arguments
     * - object: [string] the name of an object transform (case-sensitive) to lookup in the
                object registry
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
private:
    VdfnObjectRegistry *m_pReg;
};

/**
 * creates a VdfnHistoryProjectNode.
 */
class VISTADFNAPI VdfnHistoryProjectNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
    /**
     * single argument:
     * - project: [list-of-strings] determines the order and names of ports to project from
                  the history to the outport of the nodes.
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

/**
 * creates a VdfnDriverSensorNode
 */
class VISTADFNAPI VdfnDriverSensorNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:

    /**
     * the VistaDriverMap must outlive the node creator
     * @param pMap a pointer to the driver map to query for node creation
     */
    VdfnDriverSensorNodeCreate(VistaDriverMap *pMap)
    : m_pMap(pMap) {
    }

    /**
     * arguments
     * - driver: [string,mandatory] - the name of the driver that contains the sensor of desire
     * - sensor: [number,optional] - the sensor <i>index</i> to query from the driver.
                 this number may be hard to get, as it is the raw id, OR the sensor mapped id
                 if the driver supports a sensor mapping.
     * - sensor_id: [string,optional] - the sensor <i>name</i> to query from the driver.
                 the sensor must be given after the creation of the sensor using the
                 VistaDeviceSensor::SetSensorName() API.
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;

private:
    VistaDriverMap *m_pMap;
};

/**
 * template to create an instance of a VdfnAggregateNode<>.
 */
template<class T>
class VdfnAggregateNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:

    /**
     *  one argument
     *  - in: [string] - determining the property name to aggregate,
              the property must match the type T, but this is checked later (in the node-code)
     *  - order: [string] - either "newest_entry_first" oder "newest_entry_last"
	 *        determines the order in which new, defaults ro newest_entry_last
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const
	{
        try
		{
            const VistaPropertyList &oSubs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
			std::string sProp = oSubs.GetStringValue("in");
			bool bStoreNewestEntryLast = true;
			if( oSubs.GetStringValue( "order" ) == "newest_entry_first" )
				bStoreNewestEntryLast = false;
            return new TVdfnAggregateNode<T>( sProp, bStoreNewestEntryLast );
        } catch (VistaExceptionBase &x) {
            x.PrintException();
        }
        return NULL;
    }

};

/**
 * creates a TVdfnChangeDetectNode<>
 */
template<class T>
class TVdfnChangeDetectNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:

    /**
     * no argument.
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const {
        return new TVdfnChangeDetectNode<T>;
    }
};

/**
 * template code: creates a TVdfnConstantValueNode<> for type T
 * @todo check error case... memory cleanup?
 */
template<class T>
class VdfnConstantValueNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
    typedef T(*CConvFunc)(const std::string &);

    /**
     * pass a conversion function from string (proplist input) to
     * type T.
     */
    VdfnConstantValueNodeCreate(CConvFunc func)
    : m_Cf(func) {
    }

    /**
     * one argument
     * - value: [type T as string]
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const {
        TVdfnConstantValueNode<T> *pNode = new TVdfnConstantValueNode<T>;
        try {
            const VistaPropertyList &prams = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
            if (prams.HasProperty("value")) {
                pNode->SetValue(m_Cf(prams.GetStringValue("value")));
            }
        } catch (VistaExceptionBase &x) {
            x.PrintException();
        }
        return pNode;
    }
private:
    CConvFunc m_Cf;
};

/**
 * creates a VdfnLoggerNode.
 */
class VISTADFNAPI VdfnLoggerNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
    /**
     * arguments
     * - prefix: [string], mandatory: the prefix of the node file
     * - writeheader: [bool], optional, true forces to write a header as first line,
                      default false
     * - writetime: [bool], optional, true writes a timestamp a first column,
                      default false
     * - writediff: [bool], optional, true writes the timespan since the last update,
                    default false
     * - logtoconsole: [bool], optional, true writes to file and console,
                      default false
     * - sortlist: [list-of-strings], optional, determines the order of columns,
                   not all columns must be listed, those not listed will be appended
                   without order, default: empty
     * - triggerlist: [list-of-strings], optional, the list of portnames that cause
                   a log line to be produced, default: empty
     * - seperator: [string], optional, default is space ' ', "\t" is escaped to tab
     */
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

class VdfnCompositeNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:
    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const;
};

template<class T>
class TVdfnVectorDecomposeNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:

    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const {
        try {
            const VistaPropertyList &prams = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

            // defaults to false when parameter not there
            bool bInvertOrder = prams.GetBoolValue("InvertOrder");

            return new TVdfnVectorDecomposeNode<T > (bInvertOrder);
        } catch (VistaExceptionBase &x) {
            x.PrintException();
        }
        return NULL;
    }
};

template<class T>
class TVdfnCounterNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:

    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const {
        try {
            const VistaPropertyList &prams = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

            T oInitialValue = T(prams.GetDoubleValue("initial_value"));

            return new TVdfnCounterNode<T > (oInitialValue);
        } catch (VistaExceptionBase &x) {
            x.PrintException();
        }
        return NULL;
    }
};

template<class T>
class TVdfnModuloCounterNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
public:

    virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams) const {
        try {
            const VistaPropertyList &prams = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

            T oInitialValue = T(prams.GetDoubleValue("initial_value"));
            T oModulo = T(prams.GetDoubleValue("modulo"));
            if (oModulo == 0)
                oModulo = 1;

            return new TVdfnModuloCounterNode<T > (oInitialValue, oModulo);
        } catch (VistaExceptionBase &x) {
            x.PrintException();
        }
        return NULL;
    }
};


class VdfnEnvStringValueNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNNODECREATORS_H

