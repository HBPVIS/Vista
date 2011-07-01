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

#ifndef _VISTADEVICESENSOR_H
#define _VISTADEVICESENSOR_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDeviceDriversConfig.h"

#include <VistaBase/VistaVectorMath.h>
#include <VistaBase/VistaBaseTypes.h>
#include <VistaAspects/VistaReflectionable.h>
#include <VistaAspects/VistaSerializingToolset.h>
#include <vector>
#include <set>
#include <typeinfo>
#include <functional>

#include <VistaTools/VistaRingBuffer.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaSerializer;
class IVistaDeSerializer;
class VistaAtomics;
class IVistaDeviceDriver;
class IVistaDriverCreationMethod;
class VistaWindowAverageTimer;
class VistaAverageTimer;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This class defines access to a single sample of a device driver's
 * state as a blob of memory. Gives additional information such as
 * the sampling index, a sampling time stamp and the time needed
 * for the swapping of the sample. The values ''measured'' reside
 * in the m_vecMeasures. Whenever there is access to a measure of this
 * type, it is supposed to be READ ONLY, except for the driver code that
 * will write to it.
 */
class VISTADEVICEDRIVERSAPI VistaSensorMeasure
{
public:
	VistaSensorMeasure()
		: m_nMeasureIdx(0),
		  m_nMeasureTs(0),
		  m_nSwapTime(0),
		  m_nEndianess(VistaSerializingToolset::GetPlatformEndianess())
	{
	}

	VistaSensorMeasure(unsigned int nMeasureIdx,
						VistaType::microtime nTs,
						unsigned int nSize)
		:m_nMeasureIdx(nMeasureIdx),
		 m_nMeasureTs(nTs),
		 m_nSwapTime(0),
		 m_nEndianess(VistaSerializingToolset::GetPlatformEndianess()),
		 m_vecMeasures(nSize)
	{}

	VistaType::uint32        m_nMeasureIdx;
	VistaType::microtime     m_nMeasureTs;
	VistaType::microtime     m_nSwapTime;
	VistaType::uint32        m_nEndianess; //<** 0: unknown, 1: LITTLE, 2: MIDDLE, 3: BIG */
	typedef std::vector<unsigned char> MEASUREVEC;
	MEASUREVEC m_vecMeasures;
	VistaType::microtime     m_nDeliverTs; /**< age of sample in history */
    VistaType::microtime     m_nAverageDeliverTime;

    template<class T>
    const T *getRead() const
    {
    	return (const T*)( &m_vecMeasures[0] );
    }

    template<class T>
    T *getWrite()
    {
    	return (T*)( &m_vecMeasures[0] );
    }
};


/**
 * A history of samples. Defines access to all values in the history
 * (based on an index currently). It provides public, but read-only
 * members, for the sake of speed. Note that driver code can write to
 * this history at any time, so use the API provided in order to get
 * a shared and synchronized access to the critical region.
 */
class VISTADEVICEDRIVERSAPI VistaMeasureHistory
{
public:
	VistaMeasureHistory(unsigned int nHistorySize,
						 VistaAtomics *pAtomics);

	/**
	 * get a const-pointer to the sample that was recently written.
	 * Depending on your desire, this is the API to use in order to
	 * implement a LIFO access to the history buffer. This method uses
	 * this histories's write head to detect the last written sample to
	 * the history.
	 * @see GetMostCurrent()
	 */
	const VistaSensorMeasure *GetCurrentRead() const;

	/**
	 * This method uses the driver's write head to inspect which value
	 * is current. Depending on the access time, the return value is
	 * different to the GetCurrentRead(), as the history's read head
	 * is updated a bit after the driver write head is moved.
	 * @see GetCurrentRead()
	 */
	const VistaSensorMeasure *GetMostCurrent() const;

	/**
	 * Gets an old (''past'') sample from the history, dependent on a relative
	 * index as given in the arguments. GetPast(0) == GetCurrentRead().
	 * The index can be an arbitrary index (it is mod'ded in this routine),
	 * but should not be larger than the number of read-only samples in the history.
	 * This number is determined on the amount of samples that can be read during
	 * a client call (which is a user-given value) depending on the write size
	 * of the driver that is writing to this history. When not careful, it is possible
	 * to read the sample that is currently written to by the driver, which leads
	 * to possibly corrupted data.
	 *
	 * @param nNum the offset back in time starting at last value (nNum = 0)
	 * @param nCurRdPos ~0 means that the current write head position is taken
	 *        as a base (default), otherwise nNum is relative to the nCurRdPos
	 *        which has to be within [0..historyLength)
	 *
	 * @return NULL in case no data was ever swapped into visibility or taken
	 *         (i.e.: no data available), a read position in the history otherwise.
	 *         Note that it is not guaranteed that this is a meaningful measure,
	 *         it is just present in the history. Test its fields (timestamp) to
	 *         be sure the data makes sense to you.
	 */
	const VistaSensorMeasure *GetPast(unsigned int nNum, VistaType::uint32 nCurRdPos = ~0 ) const;

	/**
	 * return the current read-safe position of the latest readable container in
	 * this history.
	 * @return a usable index for GetPast() or the like, or ~0 for an invalid slot.
	           This can happen when no data was yet posted to this history.
	 */
	VistaType::uint32 GetCurrentReadPosition() const;

	/**
	 * Same as GetPast() but used the most current as a sample to start with.
	 * @see GetPast()
	 * @see GetMostCurrent()
	 */
	const VistaSensorMeasure *GetCurrentPast(unsigned int nNum) const;

	/**
	 * This API is for debug purposes (e.g. logging of the complete buffer),
	 * it will return a const reference to this history buffer, consisting
	 * of memory blobs defined by the driver.
	 */
	const TVistaRingBuffer<VistaSensorMeasure> &GetReadBuffer() const;


	// CLIENT READ ONLY API -> this might change, it is currently still
	// public for debugging purposes. SO DO NOT USE THE BELOW MEMBERS IN YOUR
	// CODE, use the above given access API instead.
	TVistaRingBuffer<VistaSensorMeasure> m_rbHistory;
	VistaType::microtime                             m_nUpdateTs; /**< strong monotonic clock value, ordinal scale at least */
	VistaType::uint32                                m_nDriverWriteSize, /**< statics, but nice for debugging, determined upon creation of the history. */
										  m_nMeasureCount, /**< the index count, e.g. the absolute number of measures */
										  m_nSwapCount, /**< the number of swaps on the history, is less or equal to measure count */
										  m_nClientReadSize, /**< statics, but nice for debugging (is determined upon creation of the history) */
										  m_nSnapshotWriteHead; /**< the fixed index for all readers of the write head in the ringed array */
private:
	VistaAtomics                        *m_pAtomics; /**< private inline instance in module */
};


class VISTADEVICEDRIVERSAPI ResolveMeasureFunctor : public std::unary_function<void,VistaSensorMeasure*>
{
	public:
		virtual const VistaSensorMeasure *operator()( void ) const = 0;
};

class VISTADEVICEDRIVERSAPI SimpleHistoryGet : public ResolveMeasureFunctor
{
public:
	typedef const VistaSensorMeasure *(VistaMeasureHistory::*func)() const;

	SimpleHistoryGet(const VistaMeasureHistory &hist, func f)
	: m_f(f)
	, m_o(hist)
	{}

	virtual const VistaSensorMeasure *operator()( void ) const
	{
		return (m_o.*m_f)();
	}

	func m_f;
	const VistaMeasureHistory &m_o;
};


/**
 * The transcode devices an access point to the individual sensor's values
 * based on the Reflectionable API from the ViSTA Aspects.
 * The idea is to have a number of fly-weight methods to get a structured access
 * to the sample / measure histories, which only define memory blobs without an
 * outer / obvious structure. The idea is furthermore to make the transcode
 * exchangeable, e.g., in order to avoid costly type conversions for the direct
 * access of the data that is recorded by the driver.
 * Although being part of the reflectionable API, it should be mentioned that
 * the preferred usage is by defining ITranscodeGet instances, which are a bit
 * more compared to the IVistaPropertyGetFunctor instances (as they return a
 * type_info structure that tells something about the type of the getter returned
 * in C++ terms.
 */
class VISTADEVICEDRIVERSAPI IVistaMeasureTranscode : public IVistaReflectionable
{
public:
	virtual ~IVistaMeasureTranscode();

	unsigned int         GetNumberOfScalars() const;
	void                 SetNumberOfScalars(unsigned int);
	bool                 SetSensorMeasure(const VistaSensorMeasure *);
	const VistaSensorMeasure *GetSensorMeasure() const;

	// ######################################################
	// REFLECTIVE API
	// ######################################################

	/**
	 * ITranscodeGet is an intermediate interface that a) rules out
	 * the usual ReflectionOperator() giving a default implementation
	 * which returns false (no conversion possible) and b) defines
	 * a full virtual GetReturnType() API to query the type_info()
	 * of the value that is supposed to be returned from the getter.
	 */
	class ITranscodeGet : public IVistaPropertyGetFunctor
	{
	public:
		ITranscodeGet(const std::string &sPropname,
					  const std::string &sClassName,
					  const std::string &sDescription = "<none>")
		: IVistaPropertyGetFunctor(sPropname, sClassName, sDescription)
		{}

		virtual ~ITranscodeGet() {}

		/**
		 *  There is no meaningful interpretation of a transcode
		 *  on the level of a property, as it can only be evaluated
		 *  with respect to a current sample. For that, a history of
		 *  data can be traversed. As a consequence, we return false
		 *  here, which results in a "no-value" output.
		 *  In a more clear language: the transcode get will not evaluate
		 *  as a property does! So do not try to use the API for TranscodeGets
		 */
		virtual bool operator()(const IVistaPropertyAwareable &,
								VistaProperty &) const
		{
			return false;
		}

		/**
		 * A transcode get is an accessor for one value of a specific type.
		 * Some API needs to determine that return value's type. This is
		 * what this API is for. In case you design your own TTranscodeGet,
		 * you must overload this API and return the type_info correctly.
		 * @todo think about avoiding RTTI
		 */
		virtual const std::type_info &GetReturnType() const = 0;
	};

	/**
	 * A convenience implementation using templates for value getters.
	 * defines API to GetValue() as an argument and on via stack.
	 * Additionally returns the type_info of the template argument (T).
	 */
	template <class T> class TTranscodeValueGet : public ITranscodeGet
	{
	public:
		TTranscodeValueGet(const std::string &sPropname,
						   const std::string &sClassName,
						   const std::string &sDescription = "<none>")
		: ITranscodeGet(sPropname, sClassName, sDescription)
		{}

		/**
		 * return T by copy on the stack, evaluate the sensor measure to
		 * do that.
		 * @return a return value as a stack copy
		 * @param the measure to evaluate
		 */
		virtual T    GetValue(const VistaSensorMeasure *)    const = 0;

		/**
		 * evaluate this getter as a side effect on the second parameter.
		 * @return true iff the second parameter contains a new value
		 * @param the measure to evaluate
		 * @param the value to store T to
		 */
		virtual bool GetValue(const VistaSensorMeasure *, T &) const = 0;

		/**
		 * generic implementation, returns the RTTI typeid()
		 */
		const std::type_info &GetReturnType() const { return typeid( T ); }
	};

	/**
	 * Convenience class to get values that are present as a vector of
	 * values by defining an API GetValueIndexed() with an index as a stack
	 * return or be side-effect.
	 */
	template <class T> class TTranscodeIndexedGet : public ITranscodeGet
	{
	public:
	TTranscodeIndexedGet(const std::string &sPropname,
						 const std::string &sClassName,
						 const std::string &sDescription = "<none>")
		: ITranscodeGet(sPropname, sClassName, sDescription)
		{}

		/**
		 * evaluate the value for the given measure for a given index,
		 * store result in T
		 * @return true iff T contains the new value, false else
		 */
		virtual bool GetValueIndexed(const VistaSensorMeasure *, T &, unsigned int nIdx ) const = 0;


		/**
		 * generic implementation, returns the RTTI type.
		 */
		const std::type_info &GetReturnType() const { return typeid( T ); }
	};

	/**
	 * return type of a vec3d (convenience)
	 */
	typedef TTranscodeValueGet<VistaVector3D>    CV3Get;

	/**
	 * return type of a quad (convenience)
	 */
	typedef TTranscodeValueGet<VistaQuaternion>  CQuatGet;

	/**
	 * return type of a double (convenience)
	 */
	typedef TTranscodeValueGet<double> CDoubleGet;

	/**
	 * return type of unsigned int (convenience)
	 */
	typedef TTranscodeValueGet<unsigned int> CUIntGet;

	/**
	 * return type of an array of doubles (convenience).
	 * Note the "scalar" in the type name in comparison to the simple
	 * typedef CDoubleGet
	 */
	typedef TTranscodeIndexedGet<double>          CScalarDoubleGet;


	/**
	 * accessor API. This method can be handy when trying to retrieve a getter
	 * function for a given property. You can think of this as an implicit
	 * cast method that will retrieve a getter from the reflectionable factory
	 * and make sure, it is a transcode get and return this to you.
	 * This method can return NULL when no getter with the correct symbol was
	 * registered or the getter is not of type ITranscodeGet.
	 * @return NULL iff no ITranscodeGet was defined for strPropName
	 */
	ITranscodeGet *GetMeasureProperty(const std::string &strPropName) const;


	std::set<std::string> GetMeasureProperties() const;

	/**
	 * @todo lookup who needs this API... there seem to be too much type string APIs
	 */
	static std::string GetTypeString();

	REFL_INLINEIMP(IVistaMeasureTranscode, IVistaReflectionable);
protected:
	IVistaMeasureTranscode();
	const VistaSensorMeasure *m_pMeasure;
	unsigned int               m_nNumberOfScalars;

	mutable std::list<std::string> m_BtList;
};


namespace TrcU
{
	template<class T>
	const T *To( const VistaSensorMeasure *m ) { return (const T*)&(*m).m_vecMeasures[0]; }

	template<class T>
	T *ToNc( VistaSensorMeasure *m ) { return (T*)&(*m).m_vecMeasures[0]; }
}

/**
 * Base class for the creation of transcoders of a
 * special type. It is supposed to be sub-classed in
 * driver code and can be claimed by an application
 * in order to create new transcoders. This is important
 * in the case that a sensor has to "adopt" data from
 * a different sensor (e.g., in cluster environments
 * when using "remote" sensors).
 */
class VISTADEVICEDRIVERSAPI IVistaMeasureTranscoderFactory
{
public:
	virtual ~IVistaMeasureTranscoderFactory() {}
	virtual IVistaMeasureTranscode *CreateTranscoder() = 0;
	virtual void DestroyTranscoder( IVistaMeasureTranscode * ) = 0;
	virtual std::string GetTranscoderName() const { return "IVistaMeasureTranscoderFactory"; }
	virtual bool OnUnload();
protected:
};

/**
 * @brief little templated helper class to simplify straightforward transcoder factory creation.
 * Speaks for itself.
 */
template<class T>
class TDefaultTranscoderFactory : public IVistaMeasureTranscoderFactory
{
public:
	TDefaultTranscoderFactory( const std::string &strName )
	: m_strName(strName) {}

	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new T;
	}

	virtual void DestroyTranscoder( IVistaMeasureTranscode *trans )
	{
		delete trans;
	}

	virtual std::string GetTranscoderName() const { return m_strName; }
private:
	std::string m_strName;
};

/**
 * @brief meta factory to be passed to creation methods to create transcoder factories for
 *        different sensor types.
 */
class VISTADEVICEDRIVERSAPI IVistaTranscoderFactoryFactory : public IVistaReferenceCountable
{
public:
	virtual ~IVistaTranscoderFactoryFactory();
	virtual IVistaMeasureTranscoderFactory *CreateFactoryForType( const std::string &strTypeName ) = 0;
	virtual void DestroyTranscoderFactory( IVistaMeasureTranscoderFactory *fac ) = 0;
};

/**
 * @brief little template helper class to simplify factory-factory creation.
 */
template<class T>
class TSimpleTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory
{
public:
	virtual IVistaMeasureTranscoderFactory *CreateFactoryForType( const std::string &strTypeName )
	{
		return new T;
	}

	virtual void DestroyTranscoderFactory( IVistaMeasureTranscoderFactory *fac )
	{
		delete fac;
	}

	static void OnUnload()
	{
		T fac;
		fac.OnUnload();
	}
};

class VISTADEVICEDRIVERSAPI ICreateTranscoder
{
public:
	virtual IVistaMeasureTranscoderFactory *Create() const = 0;
	virtual void OnUnload() = 0;
};

template <class T>
class TCreateTranscoder : public ICreateTranscoder
{
	public:
	IVistaMeasureTranscoderFactory *Create() const
	{
		return new TDefaultTranscoderFactory<T>( T::GetTypeString() );
	}

	void OnUnload()
	{
		TDefaultTranscoderFactory<T> t( T::GetTypeString() );
		t.OnUnload();
	}
};

/**
 * The VistaDeviceSensor defines access to a history of samples as taken
 * from the VistaDeviceDriver. It can be set with an arbitrary MeasureTranscode,
 * although currently this should be the one defined by the driver.
 * Users can get access to the sampling / measure history by a call to GetMeasures().
 * Think of this class as an alias to "HISTORY-OF-DATA", instead of a real,
 * physically represented sensor that is attached to a tracker device.
 */
class VISTADEVICEDRIVERSAPI VistaDeviceSensor
{
public:
	VistaDeviceSensor();
	virtual ~VistaDeviceSensor();

	IVistaDeviceDriver *GetParent() const;
	void SetParent( IVistaDeviceDriver * );

	// #############################################################
	// MEASURE API
	// #############################################################

	/**
	 * Get the history of this sensor as a read-only container.
	 * @see VistaMeasureHistory
	 */
	const VistaMeasureHistory &GetMeasures() const;

	/**
	 * Get the transcoder that this can be used with this sensor
	 * in order to get data access to the memory blob in the history
	 * for this sensor.
	 */
	IVistaMeasureTranscode *GetMeasureTranscode() const;

	/**
	 * This is "expert" API, usually a system that knows what is is
	 * doing (e.g., the driver) will call the API and set the transcoder
	 * for this sensor.
	 * @see GetMeasureTranscode()
	 */
	void SetMeasureTranscode(IVistaMeasureTranscode *pTranscode);

	/**
	 * Returns the current safe-read-revision number for the data
	 * in the history. This is the usual API to detect a need for
	 * a re-read of the data for clients. The counter is increased
	 * when two conditions are met:
	 * - the driver calls advance in the history
	 * - AND there is really new data in the history to be read off
	 * This is the usual API to detect for clients. Cache the return
	 * value of this API and compare against a new read.
	 */
	unsigned int GetDataCount() const;

	/**
	 * Returns the name that was set by using SetSensorName()
	 * @see SetSensorName()
	 */
	std::string GetSensorName() const;

	/**
	 * Simply sets a symbol to be the "name" of this sensor.
	 * This is useful for debugging, although the following accounts:
	 * - there is no namespace, users may decide to name different sensor
	     instances with the same name
	 * - there is no constraint on the name, users define a symbol
	     that is stored as a member here
	   - there is currently no API to look for a sensor with a
	     specific name in a global set, users have to define their
	     name management themselves.
	 */
	void SetSensorName( const std::string &strName );

	// ##############################################################
	// INTERMEDIATE LAYER API -- clients should not use this,
	// unless they know what they are doing.
	// ##############################################################
	/**
	 * Fixates the current read slot to match with the current
	 * write head (-1). Call this once per frame to have a stable
	 * view on the sensor data. If this API is not called, there will
	 * never be outwardly visible new data on this sensor (e.g., all
	 * "normal" tries to read data from the history will fail).
	 * @see GetDataCount()
	 * @return true if there is new data (i.e.: MEASUREINDEX changed)
	 */
	bool         SwapMeasures();

	// ##############################################################
	// DRIVER API: CLIENTS SHOULD NOT USE THE FOLLOWING API
	// ##############################################################

	/**
	 * shorthand-call to adapt the number of slots for this sensor in the
	 * sensor's history aspect.
	 * @param nSampels the absolute number of samples to be stored in the history
	 *        in terms of measures (i.e. all fields the sensor can record make 1 sample)
	 * @warning this API should not be called when the driver is running,
	 *          so make sure nobody is updating the sensor at this time.
	 */
	void         SetMeasureHistorySize(unsigned int nSamples);

	/**
	 * @return the number of slots in the history for this sensor
	 */
	unsigned int GetMeasureHistorySize() const;

	/**
	 * stores measure oMeasure as a copy in the sensor's history.
	 * This API is usually called to initialize the sensor history
	 * with default slots and should not be called by user code.
	 */
	bool         AddMeasure(const VistaSensorMeasure &oMeasure);

	/**
	 * Returns a write-able reference to the history, is only needed
	 * by drivers. Other clients should ignore this.
	 */
	VistaMeasureHistory& GetDriverMeasures();

	/**
	 * Advances the write head for drivers. Should be called by drivers
	 * only. Other clients should ignore this API.
	 */
	bool AdvanceMeasure();

	/**
	 * The UpdateTimeStamp is set by the driver code without locking.
	 * It can be used to detect a "data-ready" directly after the
	 * driver has written data to a history slot. However, this is
	 * expert API, so use it with care, especially, do not assume
	 * any information contained with the time-stamp, especially not
	 * that you have read it as an integral value.
	 * @see SetUpdateTimeStamp()
	 */
	VistaType::microtime    GetUpdateTimeStamp() const;

	/**
	 * Public API for non-public use. Ignore it unless you are a driver.
	 */
	bool         SetUpdateTimeStamp(VistaType::microtime nTs);


	/**
	 * Returns the current "measure-index", this is a property
     * that can be written by the driver concurrently, in the middle
	 * of an update process, so use it with care and if you do know
	 * what you are doing. If clients are interested in a more safe
	 * read off to see whether data has currently changed, use the
	 * GetDataCount() API. The GetMeasureIndex() API can be used
	 * for "hot-swap" detection, e.g., iff you are interested in
	 * reading the most-most-current data available.
	 * @see GetDataCount()
	 */
	unsigned int GetMeasureIndex() const;

	/**
	 * This is a hint for the cluster-creation of device sensors,
	 * the type is usually coded in the driver-factory and not
	 * in the sensor (as sensors merely alias histories).
	 * So this API is used in order to store the hint on the type
	 * in order to get the correct transcoder for the history data.
	 * Otherwise, the history remains a large memory blob for
	 * those who do not know the type of the sensor.
	 */
	unsigned int GetTypeHint() const;

	/**
	 * Stores a hint on the transcoder type of this sensor.
	 * @see GetTypeHint()
	 */
	void         SetTypeHint(unsigned int nType);

	VistaWindowAverageTimer& GetWindowTimer() const;

	// ##############################################################
	// EASY ACCESS API
	// ##############################################################
	template<class T>
	T *GetProp( const std::string &sName ) const
	{
		return dynamic_cast<T*>( GetMeasureTranscode()->GetMeasureProperty(sName) );
	}

	template<class T>
	T GetValue(    const std::string &propName,
			       const ResolveMeasureFunctor &f  ) const
	{
		IVistaMeasureTranscode::TTranscodeValueGet<T> *p
		   = GetProp< IVistaMeasureTranscode::TTranscodeValueGet<T> >( propName );
		if(p)
		{
			const VistaSensorMeasure *m = f();
			if(!m)
				throw std::exception();

			T val;
			p->GetValue( m, val );
			return val;
		}
		throw std::exception();
	}

	template<class T>
	T GetLastValue( const std::string &propName  ) const
	{
		return GetValue<T>( propName, SimpleHistoryGet( GetMeasures(), &VistaMeasureHistory::GetCurrentRead ) );
	}

	template<class T>
	T GetMostCurrentValue( const std::string &propName ) const
	{
		return GetValue<T>( propName, SimpleHistoryGet( GetMeasures(), &VistaMeasureHistory::GetMostCurrent ) );
	}

	/**
	 * @param lastRead variable to store the current read index to,
	 *        set to 0 for a first read. Make sure it is initialized as
	 *        the value will be used to calculate an offset.
	 *        The resulting value will be clamped against the maximum possible
	 *        client read size.
	 * @return the number of slots that were produced since the last
	 *         read (or in total if it was never read before).
	 */
	unsigned int GetNewMeasureCount( unsigned int &lastRead ) const;

	// ##############################################################
	// DEBUG API -- ignore
	// ##############################################################
	unsigned int GetSwap1CountFail() const;
	unsigned int GetSwap2CountFail() const;

	VistaType::microtime GetLastUpdateTs() const;

protected:
private:
	VistaAtomics          *m_pAtomics;
	IVistaMeasureTranscode *m_pMeasureTranscode;
	VistaMeasureHistory    m_rbSensorHistory;
	unsigned int            m_nSwap1FailCount,
							m_nSwap2FailCount,
							m_nTypeHint,
							m_nLastMeasureCount;
	VistaType::microtime               m_nLastUpdateTs;
	std::string             m_nSensorName;
	VistaWindowAverageTimer*
							m_pTickStop;
	IVistaDeviceDriver     *m_pParent;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADEVICESENSOR_H
