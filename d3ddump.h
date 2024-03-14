/*

#pragma once
#include <d3d9.h>
#include <D3dx9.h>
#include <Directxmath.h>

using namespace DirectX;
// Forward declarations
ID3DXFont* tempFont; // this font is used for the EndSceneHook
int DrawingText();
void DrawRect(int x, int y, int w, int h, D3DCOLOR color);
void DrawBorderBox(int x, int y, int w, int h, int t, D3DCOLOR color);
void DrawBorderBoxOut(int x, int y, int w, int h, int t, D3DCOLOR color, D3DCOLOR out);

struct Vertex_t
{
	XMFLOAT4 xyzrhw;
	D3DCOLOR color;
	enum
	{
		FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE
	};
};
LPDIRECT3DDEVICE9 pDev;

void BuildVertex(XMFLOAT4 xyzrhw, D3DCOLOR color, Vertex_t* vertexList, int index)
{
	vertexList[index].xyzrhw = xyzrhw;
	vertexList[index].color = color;
}


// this is the function used to draw text
int DrawingText()
{
	RECT Rect = { 100, 100, 200, 200 };
	tempFont->DrawTextW(NULL, L"Hello, World!", -1, &Rect, DT_CENTER | DT_NOCLIP, RED);
	return 1;
}

int PreFrame()
{
	if (tempFont)
	{
		pDev->SetFVF(Vertex_t::FVF);
	}
	return 1;
}

void DrawRect(int x, int y, int w, int h, D3DCOLOR color)
{
	Vertex_t vertexList[4];

	BuildVertex(XMFLOAT4(x, y + h, 0, 1), color, vertexList, 0);
	BuildVertex(XMFLOAT4(x, y, 0, 1), color, vertexList, 1);
	BuildVertex(XMFLOAT4(x + w, y + h, 0, 1), color, vertexList, 2);
	BuildVertex(XMFLOAT4(x + w, y, 0, 1), color, vertexList, 3);

	int vertexCount = (sizeof(vertexList) / sizeof(Vertex_t)) / 2;

	pDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, vertexCount, vertexList, sizeof(Vertex_t));
}

void DrawBorderBox(int x, int y, int w, int h, int t, D3DCOLOR color)
{
	DrawRect(x, y, w, t, color);
	DrawRect(x, y, t, h, color);
	DrawRect(x + w, y, t, h, color);
	DrawRect(x, y + h, w + t, t, color);
}

void DrawBorderBoxOut(int x, int y, int w, int h, int t, D3DCOLOR color, D3DCOLOR out)
{
	DrawBorderBox(x, y, w, h, t, color);
	DrawBorderBox(x - t, y - t, w + t * 2, h + t * 2, 1, out);
	DrawBorderBox(x + t, y + t, w - t * 2, h - t * 2, 1, out);
}

D3DCOLOR Green()
{
	D3DCOLOR Green = D3DCOLOR_RGBA(0, 255, 0, 255);
	return Green;
}

*/

/*
int D3D::FindDevice()
{
	if (!g_pDx9->GetImage())
		return 0;

	PBYTE sig = (PBYTE)FindSignature((PBYTE)g_pDx9->GetImage(), (DWORD)g_pDx9->GetImage() + g_pDx9->GetTextSize(), (PCHAR)mPattern, (PCHAR)mMask); // BaseCode + ".text" = 0x66000; shaderapidx9.dll size = 0xCE000

	if (sig)
	{
		unsigned int * ppDevice = { 0 };
		memcpy(&ppDevice, (unsigned int *)(sig + 5), 4);	// "xxxxx????" pDevice address starts at 5 bytes and is 4 bytes long
		d3ddev = (LPDIRECT3DDEVICE9)(*ppDevice);		//printf("d3ddev: %X\n", d3ddev);
		unsigned int * VTable = (unsigned int *)(*(unsigned int *)d3ddev);		//printf("VTable: %X\n", VTable);
		EndSceneAddress = &VTable[42];		//printf("EndScene address: %X\n", *EndSceneAddress);
		pTrp_EndScene = (f_EndScene)VTable[42];		//Save it to return to later in our EndScence hook
	}
	else
	{
		printf("Signature not found : %X\n", sig);
	}

	return 1;
}
*/

/*int HookVMT()
{

	while (*(g_pD3D->GetEndSceneAddr()) != (unsigned int)EndSceneHook)		// For some reason the hook has to be placed twice
	{
		void* pHookFnc = (void*)EndSceneHook;

		DWORD OldProtections = 0;
		VirtualProtect(g_pD3D->GetEndSceneAddr(), sizeof(void *), PAGE_EXECUTE_READWRITE, &OldProtections);
		memcpy(g_pD3D->GetEndSceneAddr(), &pHookFnc, sizeof(void *));
		VirtualProtect(g_pD3D->GetEndSceneAddr(), sizeof(void *), OldProtections, &OldProtections);

		Sleep(20);
	}

	return 1;
}

int UnHookVMT()
{

	while (*(g_pD3D->GetEndSceneAddr()) != (unsigned int)pTrp_EndScene)
	{
		void* pRegularFnc = (void*)pTrp_EndScene;

		DWORD OldProtections = 0;
		VirtualProtect(g_pD3D->GetEndSceneAddr(), sizeof(void *), PAGE_EXECUTE_READWRITE, &OldProtections);
		memcpy(g_pD3D->GetEndSceneAddr(), &pRegularFnc, sizeof(void *));
		VirtualProtect(g_pD3D->GetEndSceneAddr(), sizeof(void *), OldProtections, &OldProtections);

		Sleep(20);
	}

	return 1;
}*/

/*
String locations in project

xorstr:
d3d.cpp
xorstr(L"Calibri").crypt_get())

entity.cpp
xorstr("tm_leet").crypt_get()) etc.... -> check that str comparison works

menu.h -> first TEST to see if this works
xorstr(L"Skeleton").crypt_get()

module.cpp
xorstr("client_panorama.dll").crypt_get() -> constructor issue

sig.cpp
xorstr("\x89\x4C\x24\x10\xB9\x00\x00\x00\x00").crypt_get()

math.h
"\n"

*/