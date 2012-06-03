#include "VistaColor.h"
#include "VistaExceptionBase.h"

//// Constructors

VistaColor::VistaColor(
	int iColor /*= WHITE */, EFormat eFormat /*= RGB*/ )
{
	SetValues(iColor, eFormat);
}

VistaColor::VistaColor(
	int i1, int i2, int i3, EFormat eFormat /*= RGBA*/ )
{
	SetValues(i1, i2, i3, eFormat);
}

VistaColor::VistaColor(
	float f1, float f2, float f3, EFormat eFormat /*= RGB*/ )
{
	SetValues(f1, f2, f3, eFormat);
}

VistaColor::VistaColor(
	double d1, double d2, double d3, EFormat eFormat /*= RGB*/ )
{
	SetValues(d1, d2, d3, eFormat);
}

VistaColor::VistaColor(
	int i1, int i2, int i3, int i4, EFormat eFormat /*= RGBA*/ )
{
	SetValues(i1, i2, i3, i4, eFormat);
}

VistaColor::VistaColor(
	float f1, float f2, float f3, float f4, EFormat eFormat /*= RGBA*/ )
{
	SetValues(f1, f2, f3, f4, eFormat);
}

VistaColor::VistaColor(
	double d1, double d2, double d3, double d4, EFormat eFormat /*= RGBA*/ )
{
	SetValues(d1, d2, d3, d4, eFormat);
}

VistaColor::VistaColor(const float afValues[], EFormat eFormat /*= RGB*/)
{
	SetValues(afValues, eFormat);
}

VistaColor::VistaColor(
	const double adValues[], EFormat eFormat /*= RGB*/)
{
	SetValues(adValues, eFormat);
}

VistaColor::VistaColor(const int aiValues[], EFormat eFormat /*= RGB*/)
{
	SetValues(aiValues, eFormat);
}

//// Setters

void VistaColor::SetValues( int i1, int i2, int i3, EFormat eFormat /*= RGB */ )
{
	float a3fValues[3] = { 
		i1 / 255.f, 
		i2 / 255.f, 
		i3 / 255.f };
	SetValues(a3fValues, eFormat);
}

void VistaColor::SetValues(
	float f1, float f2, float f3, EFormat eFormat /*= RGB */ )
{
	float a3fValues[3] = { f1, f2, f3 };
	SetValues(a3fValues, eFormat);
}

void VistaColor::SetValues( double d1, double d2, double d3, EFormat eFormat
	/*= RGB */ )
{
	float a3fValues[3] = { 
		static_cast<float>(d1),
		static_cast<float>(d2),
		static_cast<float>(d3) };
	SetValues(a3fValues, eFormat);
}

void VistaColor::SetValues(
	int i1, int i2, int i3, int i4, EFormat eFormat /*= RGBA*/ )
{
	float a4fValues[4] = {
		i1 / 255.f,
		i2 / 255.f,
		i3 / 255.f,
		i4 / 255.f };
	SetValues(a4fValues, eFormat);
}

void VistaColor::SetValues(
	float f1, float f2, float f3, float f4, EFormat eFormat /*= RGBA*/ )
{
	float a4fValues[4] = { f1, f2, f3, f4 };
	SetValues(a4fValues, eFormat);
}

void VistaColor::SetValues(
	double d1, double d2, double d3, double d4, EFormat eFormat /*= RGBA*/ )
{
	float a4fValues[4] = { 
		static_cast<float>(d1),
		static_cast<float>(d2),
		static_cast<float>(d3),
		static_cast<float>(d4) };
	SetValues(a4fValues, eFormat);
}

void VistaColor::SetValues(
	const float afValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	switch (eFormat)
	{
	case RGB:
		m_a4fValues[0] = afValues[0];
		m_a4fValues[1] = afValues[1];
		m_a4fValues[2] = afValues[2];
		m_a4fValues[3] = 1.f;
		break;
	case RGBA:
		m_a4fValues[0] = afValues[0];
		m_a4fValues[1] = afValues[1];
		m_a4fValues[2] = afValues[2];
		m_a4fValues[3] = afValues[3];
		break;
	case HSL:
		HSLtoRGB(afValues, m_a4fValues);
		m_a4fValues[3] = 1.f;
		break;
	case HSLA:
		HSLtoRGB(afValues, m_a4fValues);
		m_a4fValues[3] = afValues[3];
		break;
	case HSV:
		HSVtoRGB(afValues, m_a4fValues);
		m_a4fValues[3] = 1.f;
		break;
	case HSVA:
		HSVtoRGB(afValues, m_a4fValues);
		m_a4fValues[3] = afValues[3];
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::SetValues(
	const double adValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
		{
			float a3fValues[3] = {
				static_cast<float>(adValues[0]),
				static_cast<float>(adValues[1]),
				static_cast<float>(adValues[2])
				};
			SetValues(a3fValues, eFormat);
		}
		break;
	case RGBA:
	case HSVA:
	case HSLA:
		{
			float a4fValues[4] = {
				static_cast<float>(adValues[0]),
				static_cast<float>(adValues[1]),
				static_cast<float>(adValues[2]),
				static_cast<float>(adValues[3])
			};
			SetValues(a4fValues, eFormat);
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::SetValues(int iColor, EFormat eFormat /*= FORMAT_RGB*/ )
{
	float a4fDecomp[4] = {
		((iColor & 0xFF000000) >> 24) / 255.0f,
		((iColor & 0x00FF0000) >> 16) / 255.0f,
		((iColor & 0x0000FF00) >> 8) / 255.0f,
		 (iColor & 0x000000FF) / 255.0f };

	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
		{
			float a3fValues[3] = {
				static_cast<float>(a4fDecomp[1]),
				static_cast<float>(a4fDecomp[2]),
				static_cast<float>(a4fDecomp[3])
			};
			SetValues(a3fValues, eFormat);
		}
		break;
	case RGBA:
	case HSVA:
	case HSLA:
		{
			float a4fValues[4] = {
				static_cast<float>(a4fDecomp[0]),
				static_cast<float>(a4fDecomp[1]),
				static_cast<float>(a4fDecomp[2]),
				static_cast<float>(a4fDecomp[3])
			};
			SetValues(a4fValues, eFormat);
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::SetValues(
	const int aiValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
		{
			float a3fValues[3] = {
				aiValues[0] / 255.f,
				aiValues[1] / 255.f,
				aiValues[2] / 255.f,
			};
			SetValues(a3fValues, eFormat);
		}
		break;
	case RGBA:
	case HSVA:
	case HSLA:
		{
			float a4fValues[4] = {
				aiValues[0] / 255.f,
				aiValues[1] / 255.f,
				aiValues[2] / 255.f,
				aiValues[3] / 255.f,
			};
			SetValues(a4fValues, eFormat);
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::SetRed(float fR)
{
	m_a4fValues[0] = fR;
}

void VistaColor::SetGreen(float fG)
{
	m_a4fValues[1] = fG;
}

void VistaColor::SetBlue(float fB)
{
	m_a4fValues[2] = fB;
}

void VistaColor::SetAlpha(float fA)
{
	m_a4fValues[3] = fA;
}

void VistaColor::SetHue(float fHue)
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	a3fHSL[0] = fHue;
	HSLtoRGB(a3fHSL, m_a4fValues);
}

void VistaColor::SetHSLSaturation(float fSaturation)
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	a3fHSL[1] = fSaturation;
	HSLtoRGB(a3fHSL, m_a4fValues);
}

void VistaColor::SetLightness(float fLightness)
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	a3fHSL[2] = fLightness;
	HSLtoRGB(a3fHSL, m_a4fValues);
}


void VistaColor::SetHSVSaturation(float fSaturation)
{
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	a3fHSV[1] = fSaturation;
	HSVtoRGB(a3fHSV, m_a4fValues);
}

void VistaColor::SetValue(float fValue)
{
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	a3fHSV[2] = fValue;
	HSVtoRGB(a3fHSV, m_a4fValues);
}

//// Getters

int VistaColor::GetValues( EFormat eFormat /*= FORMAT_RGB*/ ) const
{
	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
		{
			float a3fValues[3];
			GetValues(a3fValues, eFormat);
			return (static_cast<int>(a3fValues[0] * 255.f) << 16)
				+ (static_cast<int>(a3fValues[1] * 255.f) << 8)
				+ static_cast<int>(a3fValues[2] * 255.f);
		}
	case RGBA:
	case HSVA:
	case HSLA:
		{
			float a4fValues[4];
			GetValues(a4fValues, eFormat);
			return (static_cast<int>(a4fValues[0] * 255.f) << 24)
				+ (static_cast<int>(a4fValues[1] * 255.f) << 16)
				+ (static_cast<int>(a4fValues[2] * 255.f) << 8)
				+ static_cast<int>(a4fValues[3] * 255.f);
		}
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::GetValues(
	int aiValues[], EFormat eFormat /*= FORMAT_RGB*/) const
{
	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
		{
			float a3fValues[3];
			GetValues(a3fValues, eFormat);
			aiValues [0] = static_cast<int>(a3fValues[0] * 255.f);
			aiValues [1] = static_cast<int>(a3fValues[1] * 255.f);
			aiValues [2] = static_cast<int>(a3fValues[2] * 255.f);
		}
		break;
	case RGBA:
	case HSVA:
	case HSLA:
		{
			float a4fValues[4];
			GetValues(a4fValues, eFormat);
			aiValues [0] = static_cast<int>(a4fValues[0] * 255.f);
			aiValues [1] = static_cast<int>(a4fValues[1] * 255.f);
			aiValues [2] = static_cast<int>(a4fValues[2] * 255.f);
			aiValues [3] = static_cast<int>(a4fValues[3] * 255.f);
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::GetValues( 
	float afValues[], EFormat eFormat /*= FORMAT_RGB*/ ) const
{
	switch (eFormat)
	{
	case RGB:
		afValues[0] = m_a4fValues[0];
		afValues[1] = m_a4fValues[1];
		afValues[2] = m_a4fValues[2];
		break;
	case RGBA:
		afValues[0] = m_a4fValues[0];
		afValues[1] = m_a4fValues[1];
		afValues[2] = m_a4fValues[2];
		afValues[3] = m_a4fValues[3];
		break;
	case HSL:
		{
			float a3fValues[3];
			RGBtoHSL(m_a4fValues, a3fValues);
			afValues[0] = a3fValues[0];
			afValues[1] = a3fValues[1];
			afValues[2] = a3fValues[2];
		}
		break;
	case HSLA:
		{
			float a3fValues[3];
			RGBtoHSL(m_a4fValues, a3fValues);
			afValues[0] = a3fValues[0];
			afValues[1] = a3fValues[1];
			afValues[2] = a3fValues[2];
			afValues[3] = m_a4fValues[3];
		}
		break;
	case HSV:
		{
			float a3fValues[3];
			RGBtoHSV(m_a4fValues, a3fValues);
			afValues[0] = a3fValues[0];
			afValues[1] = a3fValues[1];
			afValues[2] = a3fValues[2];
		}
		break;
	case HSVA:
		{
			float a3fValues[3];
			RGBtoHSV(m_a4fValues, a3fValues);
			afValues[0] = a3fValues[0];
			afValues[1] = a3fValues[1];
			afValues[2] = a3fValues[2];
			afValues[3] = m_a4fValues[3];
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::GetValues( 
	double adValues[], EFormat eFormat /*= FORMAT_RGB*/ ) const
{
	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
		{
			float a3fValues[3];
			GetValues(a3fValues, eFormat);
			adValues [0] = a3fValues[0];
			adValues [1] = a3fValues[1];
			adValues [2] = a3fValues[2];
		}
		break;
	case RGBA:
	case HSVA:
	case HSLA:
		{
			float a4fValues[4];
			GetValues(a4fValues, eFormat);
			adValues [0] = a4fValues[0];
			adValues [1] = a4fValues[1];
			adValues [2] = a4fValues[2];
			adValues [3] = a4fValues[3];
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

float VistaColor::GetRed() const
{
	return m_a4fValues[0];
}

float VistaColor::GetGreen() const
{
	return m_a4fValues[1];
}

float VistaColor::GetBlue() const
{
	return m_a4fValues[2];
}

float VistaColor::GetAlpha() const
{
	return m_a4fValues[3];
}

float VistaColor::GetHue() const
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	return a3fHSL[0];
}

float VistaColor::GetHSLSaturation() const
{
	float fMin = std::min(std::min(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Min. value of RGB
	float fMax = std::max(std::max(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Max. value of RGB
	float fDelta = fMax - fMin;             //Delta RGB value

	float fL = (fMax + fMin) / 2.f; // Lightness

	float fS;

	if (fDelta == 0.f)		//This is a gray, no chroma...
	{
		fS = 0.f;
	}
	else //Chromatic data...
	{
		if (fL < .5f)
			fS = fDelta / (fMax + fMin); // Saturation
		else
			fS = fDelta / (2.f - fMax - fMin); // Saturation
	}

	return fS;
}

float VistaColor::GetLightness() const
{
	float fMin = std::min(std::min(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Min. value of RGB
	float fMax = std::max(std::max(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Max. value of RGB

	return (fMax + fMin) / 2.f;
}

float VistaColor::GetHSVSaturation() const
{
	// @todo: implement direct calculation of the saturation
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	return a3fHSV[1];
}

float VistaColor::GetValue() const
{
	// @todo: implement direct calculation of the value
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	return a3fHSV[2];
}

//// Operations

VistaColor VistaColor::Mix( const VistaColor& v3End, 
	float fFraction /*= .f5*/, EFormat eFormat /* = VistaColor::RGBA */ ) const
{
	float fOwn = 1.0f - fFraction;
	switch (eFormat)
	{
	case RGBA:
		return VistaColor( 
			fOwn * m_a4fValues[0] + fFraction * v3End.m_a4fValues[0],
			fOwn * m_a4fValues[1] + fFraction * v3End.m_a4fValues[1],
			fOwn * m_a4fValues[2] + fFraction * v3End.m_a4fValues[2],
			fOwn * m_a4fValues[3] + fFraction * v3End.m_a4fValues[3],
			RGBA);
	case RGB:
		return VistaColor( 
			fOwn * m_a4fValues[0] + fFraction * v3End.m_a4fValues[0],
			fOwn * m_a4fValues[1] + fFraction * v3End.m_a4fValues[1],
			fOwn * m_a4fValues[2] + fFraction * v3End.m_a4fValues[2],
			RGB);
	case HSVA:
		return VistaColor( 
			fOwn * GetHue() + fFraction * v3End.GetHue(),
			fOwn * GetHSVSaturation() + fFraction * v3End.GetHSVSaturation(),
			fOwn * GetValue() + fFraction * v3End.GetValue(),
			fOwn * m_a4fValues[3] + fFraction * v3End.m_a4fValues[3],
			HSVA);
	case HSV:
		return VistaColor( 
			fOwn * GetHue() + fFraction * v3End.GetHue(),
			fOwn * GetHSVSaturation() + fFraction * v3End.GetHSVSaturation(),
			fOwn * GetValue() + fFraction * v3End.GetValue(),
			HSV);
	case HSLA:
		return VistaColor( 
			fOwn * GetHue() + fFraction * v3End.GetHue(),
			fOwn * GetHSLSaturation() + fFraction * v3End.GetHSLSaturation(),
			fOwn * GetLightness() + fFraction * v3End.GetLightness(),
			fOwn * m_a4fValues[3] + fFraction * v3End.m_a4fValues[3],
			HSLA);
	case HSL:
		return VistaColor( 
			fOwn * GetHue() + fFraction * v3End.GetHue(),
			fOwn * GetHSLSaturation() + fFraction * v3End.GetHSLSaturation(),
			fOwn * GetLightness() + fFraction * v3End.GetLightness(),
			HSL);
	}
}

void VistaColor::ClampValues()
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_a4fValues[i] < 0.f) m_a4fValues[i] = 0.f;
		if (m_a4fValues[i] > 1.f) m_a4fValues[i] = 1.f;
	}
}

VistaColor& VistaColor::AddRGB(const VistaColor &oOther)
{
	m_a4fValues[0] += oOther.m_a4fValues[0];
	m_a4fValues[1] += oOther.m_a4fValues[1];
	m_a4fValues[2] += oOther.m_a4fValues[2];
	return (*this);
}

VistaColor& VistaColor::SubtractRGB(const VistaColor &oOther)
{
	m_a4fValues[0] -= oOther.m_a4fValues[0];
	m_a4fValues[1] -= oOther.m_a4fValues[1];
	m_a4fValues[2] -= oOther.m_a4fValues[2];
	return (*this);
}

VistaColor& VistaColor::MultiplyRGB(const float fScale)
{
	m_a4fValues[0] *= fScale;
	m_a4fValues[1] *= fScale;
	m_a4fValues[2] *= fScale;
	return (*this);
}

VistaColor& VistaColor::DivideRGB(const float fScale)
{
	m_a4fValues[0] /= fScale;
	m_a4fValues[1] /= fScale;
	m_a4fValues[2] /= fScale;
	return (*this);
}

//// static methods

float VistaColor::HueToRGB(float f1, float f2, float fHue)
{
	if (fHue < 0.f)
		fHue += 1.f;
	else if (fHue > 1.f)
		fHue -= 1.f;

	float fReturn;

	if ((6.f * fHue) < 1.f)
		fReturn = f1 + (f2 - f1) * 6.f * fHue;
	else if ((2.f * fHue) < 1.f)
		fReturn = f2;
	else if ((3.f * fHue) < 2.f)
		fReturn = f1 + (f2 - f1) * ((2.f / 3.f) - fHue) * 6.f;
	else
		fReturn = f1;

	return fReturn;
}

void VistaColor::HSLtoRGB(const float a3fHSL[3], float a3fRGB[3])
{
	float fH, fS, fL;
	fH = a3fHSL[0];
	fS = a3fHSL[1];
	fL = a3fHSL[2];

	if (fS == 0.0)
	{
		// saturation == 0 so use set rgb values to luminance value
		a3fRGB[0] = fL;
		a3fRGB[1] = fL;
		a3fRGB[2] = fL;
	}
	else
	{
		float f2;

		if (fL < .5f)
		{
			f2 = fL * (1.f + fS);
		}
		else
		{
			f2 = (fL + fS) - (fS * fL);
		}

		float f1 = 2.f * fL - f2;

		a3fRGB[0] = HueToRGB(f1, f2, fH + (1.f/3.f));
		a3fRGB[1] = HueToRGB(f1, f2, fH);
		a3fRGB[2] = HueToRGB(f1, f2, fH - (1.f/3.f));
	}
}

void VistaColor::RGBtoHSL(const float a3fRGB[3], float a3fHSL[3])
{
	float fR, fG, fB;
	fR = a3fRGB[0];
	fG = a3fRGB[1];
	fB = a3fRGB[2];

	float fMin = std::min(std::min(fR, fG), fB);    //Min. value of RGB
	float fMax = std::max(std::max(fR, fG), fB);    //Max. value of RGB
	float fDelta = fMax - fMin;             //Delta RGB value

	a3fHSL[2] = (fMax + fMin) / 2.f; // Luminance

	if (fDelta == 0.f)		//This is a gray, no chroma...
	{
		a3fHSL[0] = 0.f;	// Hue
		a3fHSL[1] = 0.f;	// Saturation
	}
	else                                    //Chromatic data...
	{
		if (a3fHSL[2] < .5f)
			a3fHSL[1] = fDelta / (fMax + fMin); // Saturation
		else
			a3fHSL[1] = fDelta / (2.f - fMax - fMin); // Saturation

		float deltaR = (((fMax - fR) / 6.f) + (fDelta / 2.f)) / fDelta;
		float deltaG = (((fMax - fG) / 6.f) + (fDelta / 2.f)) / fDelta;
		float deltaB = (((fMax - fB) / 6.f) + (fDelta / 2.f)) / fDelta;

		if (fR == fMax )
			a3fHSL[0] = deltaB - deltaG; // Hue
		else if (fG == fMax)
			a3fHSL[0] = (1.f / 3.f) + deltaR - deltaB; // Hue
		else if (fB == fMax)
			a3fHSL[0] = (2.f / 3.f) + deltaG - deltaR; // Hue

		if (a3fHSL[0] < 0.f)
			a3fHSL[0] += 1.f; // Hue
		else if (a3fHSL[0] > 1.f)
			a3fHSL[0] -= 1.f; // Hue
	}
}

void VistaColor::HSLtoHSV(const float a3fHSL[3], float a3fHSV[3])
{
	a3fHSV[0] = a3fHSL[0];
	float fL = a3fHSL[2] * 2.f;
	float fS = a3fHSL[1] * ((fL <= 1.f) ? fL : 2.f - fL);
	a3fHSV[2] = (fL + fS) / 2.f;
	if (fL + fS > 0.f)
	{
		a3fHSV[1] = (2.f * fS) / (fL + fS);
	}
	else
	{
		a3fHSV[1] = 0.f;
	}
}

void VistaColor::HSVtoHSL(const float a3fHSV[3], float a3fHSL[3])
{
	a3fHSL[0] = a3fHSV[0];
	float fL = (2.f - a3fHSV[1]) * a3fHSV[2];
	a3fHSL[1] = a3fHSV[1] * a3fHSV[2];
	if (fL == 2.f)
	{
		a3fHSL[1] = 0.f;
	}
	else if (fL > 0.f)
	{
		a3fHSL[1] /= (fL <= 1.f) ? fL : (2.f - fL);
	}
	else
	{
		a3fHSL[1] = 0.f;
	}
	a3fHSL[2] = fL / 2.f;
}

void VistaColor::HSVtoRGB(const float a3fHSV[3], float a3fRGB[3])
{
	// @todo: implement direct conversion
	float a3fHSL[3];
	HSVtoHSL(a3fHSV, a3fHSL);
	HSLtoRGB(a3fHSL, a3fRGB);
}

void VistaColor::RGBtoHSV(const float a3fRGB[3], float a3fHSV[3])
{
	// @todo: implement direct conversion
	float a3fHSL[3];
	RGBtoHSL(a3fRGB, a3fHSL);
	HSLtoHSV(a3fHSL, a3fHSV);
}
