#pragma once
#include <d3d9.h>
#include <D3dx9.h>
#include <Directxmath.h>
#include "sig.h"
#include "color.h"
#include "math.h"
#include "static.h"
#include <vector>

// include the Direct3D Library "d3d9.lib" and "D3dx9.lib" in Linker > Input > Additional dependencies
// You cannot cast a pointer-to-member to void * or to any other "regular" pointer type. Pointers-to-members are not addresses the way regular pointers are. 
// Thus, declaring the functions outside of the D3D class

using namespace DirectX;
// Forward declarations
typedef long(__stdcall * f_EndScene)(IDirect3DDevice9 * pDevice);
HRESULT __stdcall EndSceneHook(IDirect3DDevice9 * pDevice);
LRESULT __stdcall NewWndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
int HookVMT();
int UnHookVMT();

enum eFonts
{
	fDistance,
	fName,
	fMenu,
	fBones
};

class D3D
{
private:
	LPDIRECT3DDEVICE9 d3ddev;	// the pointer to the device class
	//HWND m_hWnd;
	std::vector<ID3DXFont*> pFonts;		// the pointer to the created font 
	int m_nSize[2];
	long OldWndProc;
	//unsigned int* EndSceneAddress;	// the pointer to VTable[42] (EndScene address)
	struct Vertex_t
	{
		XMFLOAT4 xyzrhw;
		D3DCOLOR color;
		enum
		{
			FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE
		};
	};

public:
	D3D();
	~D3D();

	int InitD3D();
	int InitEndScene();
	int IniFonts();
	ID3DXFont* CreateDrawingFont(int height, std::wstring font_name);
	int SetHwnd();
	int SetWdnProc();
	int SetResolution();
	int EnableAlphaBlending();
	int DisableAlphaBlending();
	int PreFrame();
	int DrawingText(int l, int t, int r, int b, std::wstring text, Color color, ID3DXFont* pFont, int formatFlag = 1);
	int DrawRect(int x, int y, int w, int h, Color color);
	int DrawBorderBox(int x, int y, int w, int h, int t, Color color);
	int DrawBorderBoxOut(int x, int y, int w, int h, int t, Color color, Color out);
	int DrawLine(int x0, int y0, int x1, int y1, Color color);
	int DrawCheckBox(int x, int y, int w, int h, int t, Color color, Color out);
	int DrawBone(Vector3 start, Vector3 end, Color color);
	int DrawSkeleton(Ent * pEnt, Color color);
	int DrawHealthBar(int x, int y, int h, int hp);
	int DrawEsp(Ent * pEnt, int entNb);
	int PrintBones(Ent * pEnt);


	LPDIRECT3DDEVICE9 GetD3DDev()
	{
		return d3ddev;
	}

	/*HWND GetHwnd()
	{
		return m_hWnd;
	}*/

	ID3DXFont* GetFont(int index)
	{
		return pFonts[index];
	}

	void GetScreenSize(int* width, int* height)
	{
		if (width)
			*width = m_nSize[0];

		if (height)
			*height = m_nSize[1];
	}

	long GetOldWndProc()
	{
		return OldWndProc;
	}

	//unsigned int * GetEndSceneAddr()
	//{
	//	return EndSceneAddress;
	//}

	void BuildVertex(XMFLOAT4 xyzrhw, D3DCOLOR color, Vertex_t* vertexList, int index)
	{
		vertexList[index].xyzrhw = xyzrhw;
		vertexList[index].color = color;
	}
};