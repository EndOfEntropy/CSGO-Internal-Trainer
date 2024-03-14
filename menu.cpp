#include "d3d.h"
#include "menu.h"


// Menu class
Menu* g_pMenu = new Menu();

// Forward declarations
extern D3D* g_pD3D;

// Menu class functions

Menu::Menu()
{
	m_showMenu = 0;
	m_menuItems = Items();
	m_x = 0;
	m_y = 0;
	m_w = 200;
	m_h = m_menuItems.size() * 30 + 30;
	m_cursorx = 0;
	m_cursory = 0;
}

Menu::~Menu()
{
}

int Menu::ShowHideMenu()
{
	m_showMenu = !m_showMenu;

	return 1;
}

// Menu pos depends on the screen size, thus cannot be in the constructor
int Menu::SetMenuPos()
{
	g_pD3D->GetScreenSize(&m_x, &m_y);
	m_x = m_x / 4 - 100;
	m_y = m_y / 2 - 100;

	return 1;
}

int Menu::DrawMenu()
{
	g_pD3D->EnableAlphaBlending();
	g_pD3D->DrawCheckBox(m_x, m_y, m_w, m_h, 2, Color::AlphaSilver(), Color::Black());
	g_pD3D->DisableAlphaBlending();

	DrawItems();
	MouseInput();

	return 1;
}

int Menu::DrawItems()
{
	for (auto& item : m_menuItems)
	{
		g_pD3D->DrawingText(m_x + 30, m_y + (item.m_value * 30), 10, 10, item.m_name, Color::Black(), g_pD3D->GetFont(fMenu), DT_LEFT);
		
		if (IsBoxClicked(m_x + m_w - 40, m_y + (item.m_value * 30) - 5, 15, 15))
		{
			item.m_status = !item.m_status;			
		}

		if (item.m_status)
		{
			g_pD3D->DrawCheckBox(m_x + m_w - 40, m_y + (item.m_value * 30) - 5, 15, 15, 1, Color::White(), Color::Black());
		}
		else
		{
			g_pD3D->DrawCheckBox(m_x + m_w - 40, m_y + (item.m_value * 30) - 5, 15, 15, 1, Color::DimGrey(), Color::Black());
		}
	}

	return 1;
}

int Menu::MouseInput()
{
	POINT Pos;
	RECT myRect;
	D3DDEVICE_CREATION_PARAMETERS d3dpp;
	GetCursorPos(&Pos);
	g_pD3D->GetD3DDev()->GetCreationParameters(&d3dpp);
	GetWindowRect(d3dpp.hFocusWindow, (LPRECT)&myRect);

	m_cursorx = Pos.x - 03 - myRect.left;
	m_cursory = Pos.y - 26 - myRect.top;

	//DRAW MOUSE
	g_pD3D->DrawLine(m_cursorx - 5, m_cursory - 5, m_cursorx + 5, m_cursory + 5, Color::Red());
	g_pD3D->DrawLine(m_cursorx - 5, m_cursory + 5, m_cursorx + 5, m_cursory - 5, Color::Red());

	if (GetAsyncKeyState(0x1) & 1)
	{
		m_click = true;
	}
	else
	{
		m_click = false;
	}
	if (GetAsyncKeyState(0x1))
	{
		m_holdclick = true;
	}
	else
	{
		m_holdclick = false;
	}

	return 1;
}

bool Menu::IsBoxClicked(int x, int y, int w, int h)
{
	if (m_cursorx > x && m_cursory > y  && m_cursorx < x + w && m_cursory < y + h && m_click)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}