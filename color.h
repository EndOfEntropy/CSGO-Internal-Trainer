#pragma once

#include "d3d.h"

class Color
{
public:
	Color()
	{
		m_r = m_g = m_b = m_a = 0;
		m_code = 0;
	}

	Color(BYTE r, BYTE g, BYTE b, BYTE a = 255)
	{
		m_r = r;
		m_g = g;
		m_b = b;
		m_a = a;
		m_code = D3DCOLOR_RGBA(r, g, b, a);
	}

	Color(D3DCOLOR code)
	{
		m_code = code;
	}

	BYTE R() const
	{
		return m_r;
	}
	BYTE G() const
	{
		return m_g;
	}
	BYTE B() const
	{
		return m_b;
	}
	BYTE A() const
	{
		return m_a;
	}

	D3DCOLOR Code() const
	{
		return m_code;
	}

	static Color None()
	{
		return Color(0, 0, 0, 0);
	}
	static Color White()
	{
		return Color(255, 255, 255);
	}
	static Color Black()
	{
		return Color(0, 0, 0);
	}
	static Color DimGrey()
	{
		return Color(64, 64, 64);
	}
	static Color Grey()
	{
		return Color(128, 128, 128);
	}
	static Color Silver()
	{
		return Color(192, 192, 192);
	}
	static Color Red()
	{
		return Color(255, 0, 0);
	}
	static Color Green()
	{
		return Color(0, 255, 0);
	}
	static Color Blue()
	{
		return Color(0, 0, 255);
	}
	static Color Cyan()
	{
		return Color(0, 128, 255);
	}
	static Color Yellow()
	{
		return Color(255, 255, 0);
	}
	static Color Purple()
	{
		return Color(144, 95, 215);
	}
	static Color Pink()
	{
		return Color(255, 105, 180);
	}
	static Color Salmon()
	{
		return Color(233, 62, 51);
	}
	static Color AlphaSilver()
	{
		return Color(192, 192, 192, 100);
	}

private:
	BYTE		m_r, m_g, m_b, m_a;
	D3DCOLOR	m_code;
};