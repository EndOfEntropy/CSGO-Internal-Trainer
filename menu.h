#pragma once
#include <d3d9.h>
#include <D3dx9.h>
#include <Directxmath.h>
#include <vector>
#include "math.h"
#include "color.h"
#include "xorstr.h"


using namespace DirectX;
// Forward declarations
enum eItems
{
	iSkeleton,
	iBox,
	iHealth,
	iDistance,
	iName,
	Max_eItems
};

struct Item
{
	std::wstring m_name;
	int m_value;
	bool m_status;
	//bool istitle;
};

class Menu
{
private:
	bool m_showMenu, m_click, m_holdclick;
	int m_x, m_y, m_w, m_h;
	int m_cursorx, m_cursory;
	std::vector<Item> m_menuItems;

public:
	Menu();
	~Menu();

	int ShowHideMenu();
	int SetMenuPos();
	int DrawMenu();
	int	DrawItems();
	int MouseInput();
	bool IsBoxClicked(int x, int y, int w, int h);

	bool GetShowMenu()
	{
		return m_showMenu;
	}

	void GetMenuPos(int* x, int* y)
	{
		if (x)
			*x = m_x;

		if (y)
			*y = m_y;
	}

	void GetMenuSize(int* w, int* h)
	{
		if (w)
			*w = m_w;

		if (h)
			*h = m_h;
	}

	void GetCursorPosition(int* x, int* y)
	{
		if (x)
			*x = m_cursorx;

		if (y)
			*y = m_cursory;
	}

	bool GetItemStatus(int itemNb)
	{
		return m_menuItems[itemNb].m_status;
	}

	std::vector<Item> Items()
	{
		std::vector<Item> items;
		items.push_back({ xorstr(L"Skeleton").crypt_get(), 1, 0});
		items.push_back({ xorstr(L"Box").crypt_get(), 2 , 0 });
		items.push_back({ xorstr(L"Health").crypt_get(), 3, 0 });
		items.push_back({ xorstr(L"Distance").crypt_get(), 4, 0 });
		items.push_back({ xorstr(L"Name").crypt_get(), 5, 0 });

		return items;
	}

};
