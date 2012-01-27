/*============================================================================*/
/*                    ViSTA VR toolkit - OpenCV2 driver                  */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published         */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
// $Id: VistaOpenCVCaptureDriver.h 5761 2011-01-17 19:46:22Z ingoassenmacher $

#ifndef __VISTAOPENCVCAPTUREDRIVER_H
#define __VISTAOPENCVCAPTUREDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

#if defined(WIN32) && !defined(VISTAOPENCVCAPTUREDRIVER_STATIC) 
#ifdef VISTAOPENCVCAPTUREDRIVER_EXPORTS
#define VISTAOPENCVCAPTUREDRIVERAPI __declspec(dllexport)
#else
#define VISTAOPENCVCAPTUREDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAOPENCVCAPTUREDRIVERAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverInfoAspect;
class VistaDriverThreadAspect;

namespace cv
{
	class VideoCapture;
}

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAOPENCVCAPTUREDRIVERAPI VistaOpenCVCaptureDriver : public IVistaDeviceDriver
{
public:

	struct _videotype
	{
		_videotype()
		{
		}
	};

	VistaOpenCVCaptureDriver(IVistaDriverCreationMethod *);
	~VistaOpenCVCaptureDriver();

	bool Connect();



	class Parameters : public VistaDriverGenericParameterAspect::IParameterContainer
	{
		REFL_DECLARE
	public:
		Parameters( VistaOpenCVCaptureDriver* pDriver );

		enum
		{
			MSG_CAPTUREDEVICE_CHG = VistaDriverGenericParameterAspect::IParameterContainer::MSG_LAST,
			MSG_REQUESTEDWIDTH_CHG,
			MSG_REQUESTEDHEIGHT_CHG,
			MSG_WIDTH_CHG,
			MSG_HEIGHT_CHG,
			MSG_FORMAT_CHG,
			MSG_FRAMERATE_CHG,
			MSG_DOESHW2RGB_CHG,
			MSG_FROMFILE_CHG,
			MSG_WANTRGB_CHG,
			MSG_LAST
		};

		int GetCaptureDevice() const;
		bool SetCaptureDevice( int );

		int GetWidthRequested() const;
		bool SetWidthRequested( int width );

		int GetHeightRequested() const;
		bool SetHeightRequested( int height );

		int GetWidth() const;
		bool SetWidth( int width );

		int GetHeight() const;
		bool SetHeight( int height );

		double GetFormat() const;
		bool SetFormat( double );

		int GetFrameRate() const;
		bool SetFrameRate( int );

		bool GetDoesHW2RGB() const;
		bool SetDoesHW2RGB( bool );

		bool GetWantRGB() const;
		bool SetWantRGB( bool );

		std::string GetFromFile() const;
		bool SetFromFile( const std::string & );

		size_t GetMemorySize() const;

		virtual bool TurnDef( bool def );
		virtual bool Apply();

	private:
		VistaOpenCVCaptureDriver *m_parent;

		int m_device,
		    m_widthRQ,
		    m_heightRQ,
		    m_width,
		    m_height,
		    m_framerate;
		bool m_bDef, m_doesHw2RGB, m_bwantRGB;
		double m_format;

		std::string m_fromFile;
	};

	void SignalPropertyChanged(int msg);
protected:

	virtual bool PhysicalEnable(bool bEnable);
	bool DoSensorUpdate(VistaType::microtime nTs);
private:
	void OnReconnectCamera();
	void OnUpdateMeasureSize();

	VistaDriverInfoAspect       *m_pInfo;
	VistaDriverThreadAspect     *m_pThread;

	VistaDriverGenericParameterAspect *m_pParams;

	cv::VideoCapture *m_pCapture;
};


class VISTAOPENCVCAPTUREDRIVERAPI VistaOpenCVCaptureDriverCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaOpenCVCaptureDriverCreationMethod(IVistaTranscoderFactoryFactory *fac);
	virtual IVistaDeviceDriver *CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif //__VISTAOPENCVCAPTURE_H

