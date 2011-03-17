/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2009 RWTH Aachen University               */
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

#ifndef _VISTATIMER_H
#define _VISTATIMER_H

#include "VistaBaseConfig.h"
#include "VistaBaseTypes.h"

#include <vector>

class IVistaTimerImp;

class VISTABASEAPI VistaTimer
{
public:
	VistaTimer( IVistaTimerImp *pImp = NULL );
	virtual ~VistaTimer();

	/**
	 * @return time value with arbitrary reference time,
	 *         high precision (10^-6s to 10^-7s),
	 *         should be strictly monotonic rising most of the time
	 *         (changes every 1microsec (LINUX) or 0.1microsec(WIN32))
	 */
	microtime  GetMicroTime() const;

	/**
	 * @return Unspecified strictly monotonic rising Timestamp
	 */
	microstamp GetMicroStamp() const;

	/**
	 * @return time since 01.01.1970,
	 *         medium precision (10^-5s / 10 microsecond)
	 *         (not necessaryly strictly) monotonic rising
	 */
	systemtime GetSystemTime() const;

	/**
	 * @param  microtime value, as received from GetMicrotime(), i.e. unaltered.
	 *         NOTE: The microtime has to be retrieved from the same
	 *         timer implementation, so take care that it doesnt change inbetween
	 * @return a monotonic increasing time value
	 *         representing system time (i.e. secs.msecs since 1970)
	 */
	systemtime ConvertToSystemTime( const microtime mtTime ) const;

	/**
	 * @return Life time of the Timer, i.e. time since 
	 *         creation or last call to ResetLifeTimer()
	 */
	microtime GetLifeTime() const;
	/**
	 * resets Timer's life time to zero
	 */
	void ResetLifeTime();

	/**
	 * @return The standard timer singleton, for easy time access.
	 *         If no timer instance is available yet, a new one will
	 *         be created using the current TimerImp singleton.
	 *         NOTE: If no default timer imp is set, an invalid
	 *         Timer will be created!
	 */
	static const VistaTimer& GetStandardTimer();
protected:
	IVistaTimerImp* m_pImp;
	microtime       m_nBirthTime;
};


class VISTABASEAPI VistaAverageTimer : public VistaTimer
{
public:
	VistaAverageTimer( IVistaTimerImp *pImp = NULL );
	virtual ~VistaAverageTimer();

	/**
	 * Starts the recording and sets the reference time used by RecordTime().
	 */
	virtual void StartRecording();
	/**
	 * Records the current time different and adds it to the
	 * averaging time span. If a StartRecording has been called before,
	 * the time span is the difference to this start, otherwise the
	 * time of the last StartRecording() call (or the timer creation/reset) is used
	 */
	virtual void RecordTime();		

	/**
	 * @return time since record started (or since last record, or timer creation/reset)
	 */
	microtime GetCurrentRecordingTime() const;
	
	virtual microtime GetAccumulatedTime() const;
	virtual int GetRecordCount() const;
	virtual microtime GetAverageTime() const;				

	virtual void ResetAveraging();

protected:
	microtime	m_nRecordStartTime;
	microtime	m_nAccumulatedTime;
	microtime	m_nAverage;
	int			m_iCount;
};


/**
 * Average filter, where the average time is not calculated form
 * total_time / total_count, but by a weighted average, so that
 * average =  ( wheight * new_record + old_average ) / ( wheight + 1 )
 * This way, old values have a more and more diminishing influence,
 * while newer ones are weighted stronger
 */
class VISTABASEAPI VistaWeightedAverageTimer : public VistaAverageTimer
{
public:
	VistaWeightedAverageTimer( const float fWeight = 1,
								IVistaTimerImp *pImp = NULL );
	virtual ~VistaWeightedAverageTimer();

	float GetWeight() const;
	void SetWeight( const float fWeight );

	/**
	 * Records the current time different and adds it to the
	 * averaging time span. If a StartRecording has been called before,
	 * the time span is the difference to this start, otherwise the
	 * time of the last StartRecording() call (or the timer creation/reset) is used
	 */
	virtual void RecordTime();		

	virtual void ResetAveraging();

protected:
	float		m_fWeight;
	float		m_fNorm;
};

/**
 * Average filter, where the average time is not calculated form
 * total_time / total_count, but by a weighted average, so that
 * average =  ( weight * new_record + old_average ) / ( weight + 1 )
 * This way, old values have a more and more diminishing influence,
 * while newer ones are weighted stronger
 */
class VISTABASEAPI VistaWindowAverageTimer : public VistaAverageTimer
{
public:
	VistaWindowAverageTimer( const int iWindowSize = 10,
								IVistaTimerImp *pImp = NULL );
	virtual ~VistaWindowAverageTimer();

	int GetWindowSize() const;

	/**
	 * Records the current time different and adds it to the
	 * averaging time span. If a StartRecording has been called before,
	 * the time span is the difference to this start, otherwise the
	 * time of the last StartRecording() call (or the timer creation/reset) is used
	 */
	virtual void RecordTime();		

	virtual void ResetAveraging();

protected:
	std::vector<microtime>	m_vecRecords;
	int						m_iCurrentSlot;
};



//class VISTABASEAPI VistaTickStop : public VistaTimer
//{
//public:
//	typedef std::vector<microtime>::size_type index_type;

//	VistaTickStop( index_type nWindowLength = 10,
//					IVistaTimerImp *pImp = NULL );
//	virtual ~VistaTickStop();

//	void      Tick();

//	double    GetFrequency() const;
//	double    GetWindowFrequency() const;
//	microtime GetAverageDelta() const;
//	microtime GetAgeFromLast() const;

//	void ResetTickStop();

//private:
//	std::vector<microtime> m_vTicks;
//	index_type m_nTickIdx;
//	uint64 m_nTickCount;
//	microtime m_nAccumMt, m_nLastMt;
//};

////@todo make this a timer, maybe fix the result 
//// (equal weight for measures, not declining with age)
//class VISTABASEAPI VistaWeightedAverageTimer
//{
//public:
//	VistaWeightedAverageTimer();

//	inline void NewDt( microtime dt )
//	{
//		if(m_nAvgTime == 0)
//			m_nAvgTime = dt;
//		else
//		{
//			m_nAvgTime = (m_nAvgTime + dt) / 2.0;
//		}
//	}


//	microtime GetAverage() const;
//private:
//	microtime m_nAvgTime;
//};

#endif /* _VISTATIMER_H */
