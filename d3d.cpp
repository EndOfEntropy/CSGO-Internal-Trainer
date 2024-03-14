#include "d3d.h"
#include "module.h"
#include "engine.h"
#include "entity.h"
#include "menu.h"
#include "datahook.h"
#include "xorstr.h"


// D3D class = renderer
D3D* g_pD3D = new D3D();
// Forward declarations
extern Entity* g_pEntity;
extern Entity* g_pPlayer;
extern Engine* g_pEngine;
extern Static* g_pStatic;
extern Menu* g_pMenu;
extern DataHook* g_pDataHook;
f_EndScene pTrp_EndScene;


HRESULT __stdcall EndSceneHook(IDirect3DDevice9 * pDevice)
{
	g_pD3D->PreFrame(); // dont remove from the hook function
	int entNb = 1;

	if (g_pMenu->GetShowMenu())
		g_pMenu->DrawMenu();

	for (const auto &cltInf : g_pStatic->pCBaseEntList->entList)
	{
		if (!cltInf.entptr)
			continue;

		g_pPlayer->UpdateEntity((uintptr_t)g_pStatic->pLocalPlayer);
		g_pEntity->UpdateEntity((uintptr_t)cltInf.entptr);

		if (g_pEntity->IsValid() && (g_pPlayer->GetTeamNum() != g_pEntity->GetTeamNum())) // Draw if valid and != team
		{
			g_pD3D->DrawEsp(cltInf.entptr, entNb);	// g_pD3D->PrintBones(cltInf.entptr);
			entNb++;
		}
	}

	return pTrp_EndScene(pDevice);		// return *This pointer to the original EndScene address
}

int HookVMT()
{
	while (*(g_pDataHook->GetNewEndScene()) != (uintptr_t)EndSceneHook)		// For some reason the hook has to be placed twice
	{
		void* pHookFnc = (void*)EndSceneHook;

		memcpy(g_pDataHook->GetNewEndScene(), &pHookFnc, sizeof(void *));

		Sleep(20);
	}

	return 1;
}

int UnhookVMT()
{
	while (*(g_pDataHook->GetNewEndScene()) != (uintptr_t)pTrp_EndScene)
	{
		void* pRegularFnc = (void*)pTrp_EndScene;

		memcpy(g_pDataHook->GetNewEndScene(), &pRegularFnc, sizeof(void *));

		Sleep(20);
	}

	return 1;
}


// WndProc message interception
LRESULT __stdcall NewWndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_INSERT:
			g_pDataHook->VftHook();		// For some reason the hook has to be placed twice. Because we intercept the messages no reset happen while hooking, thus Insert has to be pressed twice
			break;
		case VK_DELETE:
			g_pDataHook->VftUnhook();
			break;
		case VK_END:
			g_pMenu->ShowHideMenu();
			break;
		}
		break;
	}
	return CallWindowProc((WNDPROC)g_pD3D->GetOldWndProc(), Hwnd, Message, wParam, lParam);
}

// D3D class functions

D3D::D3D()
{
}

D3D::~D3D()
{
}

int D3D::InitD3D()
{
	InitEndScene();
	IniFonts();
	SetHwnd();	// Empty
	SetWdnProc();
	SetResolution();	// Called once, res change = bug
	HookVMT();
	
	return 1;
}

int D3D::InitEndScene()
{
	d3ddev = (LPDIRECT3DDEVICE9)*(uintptr_t**)(g_pDataHook->GetClassBase());
	pTrp_EndScene = (f_EndScene)(*g_pDataHook->GetOldEndScene());

	return 1;
}

int D3D::IniFonts()
{
	pFonts.push_back({ CreateDrawingFont(20, xorstr(L"Calibri").crypt_get()) });	// Distance 
	pFonts.push_back({ CreateDrawingFont(20, xorstr(L"Calibri").crypt_get()) });	// Name
	pFonts.push_back({ CreateDrawingFont(20, xorstr(L"Calibri").crypt_get()) });	// Menu
	pFonts.push_back({ CreateDrawingFont(14, xorstr(L"Calibri").crypt_get()) });	// Bones

	return 1;
}

// this is the function used to create a font
ID3DXFont* D3D::CreateDrawingFont(int height, std::wstring fontName)
{
	ID3DXFont* pFont;
	D3DXFONT_DESC fontDesc;
	fontDesc.Height = height;
	fontDesc.Width = 0;
	fontDesc.Weight = 0;
	fontDesc.MipLevels = 1;
	fontDesc.Italic = false;
	fontDesc.CharSet = DEFAULT_CHARSET;
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality = DEFAULT_QUALITY;
	fontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	wcscpy_s(fontDesc.FaceName, fontName.c_str());

	if (d3ddev == NULL)
	{
		//cout << "Device to create font on is NULL. \n";
		return 0;
	}

	D3DXCreateFontIndirect(d3ddev, &fontDesc, &pFont);
	return pFont;
}

int D3D::SetHwnd()
{
	/*if (d3ddev->GetCreationParameters(&g_pD3D->d3dpp) != D3D_OK)
	{
		cout << "GetCreationParameters failed ! \n";
		return 0;
	}

	m_hWnd = d3dpp.hFocusWindow;
	if (m_hWnd == NULL)
	{
		cout << "m_hWnd is NULL ! \n";
		return 0;
	}*/

	return 1;
}

int D3D::SetWdnProc()
{
	D3DDEVICE_CREATION_PARAMETERS d3dpp;
	d3ddev->GetCreationParameters(&d3dpp);
	OldWndProc = SetWindowLongPtr(d3dpp.hFocusWindow, GWL_WNDPROC, (LONG_PTR)NewWndProc);
	if (OldWndProc == 0)
	{
		//cout << "SetWindowLongPtr failed ! \n";
		return 0;
	}
		
	return 1;
}

int D3D::SetResolution()
{
	RECT client;	// game = client + non-client
	D3DDEVICE_CREATION_PARAMETERS d3dpp;
	d3ddev->GetCreationParameters(&d3dpp);
	
	if (!GetClientRect(d3dpp.hFocusWindow, &client))
	{
		return 0;
	}

	m_nSize[0] = client.right;	// game.right - game.left;
	m_nSize[1] = client.bottom;	// game.bottom - game.top;

	return 1;
}

int D3D::EnableAlphaBlending()
{
	d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	return 1;
}

int D3D::DisableAlphaBlending()
{
	d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	return 1;
}

int D3D::PreFrame()
{
	if (d3ddev)
	{
		d3ddev->SetFVF(Vertex_t::FVF);
	}
	return 1;
}

int D3D::DrawingText(int x, int y, int w, int h, std::wstring text, Color color, ID3DXFont* pFont, int formatFlag)
{
	if (!pFont)
		return 0;

	RECT Rect = { x - w * 0.5, y - h * 0.5, x + w * 0.5, y + h * 0.5 };
	pFont->DrawTextW(NULL, text.c_str(), -1, &Rect, formatFlag | DT_VCENTER | DT_NOCLIP, color.Code());

	return 1;
}

int D3D::DrawRect(int x, int y, int w, int h, Color color)
{
	Vertex_t vertexList[4];

	BuildVertex(XMFLOAT4(x, y + h, 0, 1), color.Code(), vertexList, 0);
	BuildVertex(XMFLOAT4(x, y, 0, 1), color.Code(), vertexList, 1);
	BuildVertex(XMFLOAT4(x + w, y + h, 0, 1), color.Code(), vertexList, 2);
	BuildVertex(XMFLOAT4(x + w, y, 0, 1), color.Code(), vertexList, 3);

	int vertexCount = (sizeof(vertexList) / sizeof(Vertex_t)) / 2;
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, vertexCount, vertexList, sizeof(Vertex_t));

	return 1;
}

int D3D::DrawBorderBox(int x, int y, int w, int h, int t, Color color)
{
	DrawRect(x, y, w, t, color);
	DrawRect(x, y, t, h, color);
	DrawRect(x + w, y, t, h, color);
	DrawRect(x, y + h, w + t, t, color);

	return 1;
}

int D3D::DrawBorderBoxOut(int x, int y, int w, int h, int t, Color color, Color out)
{
	DrawBorderBox(x, y, w, h, t, color);
	DrawBorderBox(x - t, y - t, w + t * 2, h + t * 2, 1, out);
	DrawBorderBox(x + t, y + t, w - t * 2, h - t * 2, 1, out);

	return 1;
}

int D3D::DrawLine(int x0, int y0, int x1, int y1, Color color)
{
	Vertex_t vertexList[2];

	BuildVertex(XMFLOAT4(x0, y0, 0, 0), color.Code(), vertexList, 0);
	BuildVertex(XMFLOAT4(x1, y1, 0, 0), color.Code(), vertexList, 1);

	int vertexCount = (sizeof(vertexList) / sizeof(Vertex_t));
	d3ddev->DrawPrimitiveUP(D3DPT_LINELIST, vertexCount, vertexList, sizeof(Vertex_t));

	return 1;
}

int D3D::DrawCheckBox(int x, int y, int w, int h, int t, Color color, Color out)
{
	DrawRect(x, y, w, h, color);
	DrawBorderBox(x, y, w, h, t, out);

	return 1;
}

int D3D::DrawBone(Vector3 start, Vector3 end, Color color)
{
	Vector3 vStart, vEnd;
	if ((!g_pEngine->WorldToScreen(start, vStart)) || (!g_pEngine->WorldToScreen(end, vEnd)))
		return 0;

	DrawLine(vStart.x, vStart.y, vEnd.x, vEnd.y, color);

	return 1;
}

int D3D::DrawSkeleton(Ent * pEnt, Color color)
{
	
	const std::vector<std::vector<int>> bones = g_pEntity->GetBoneStruct(g_pEntity->GetModelName(pEnt));

	for (auto& skel : bones)
	{
		for (auto bone = skel.begin(); bone != skel.end() - 1; bone++)
		{
			DrawBone(g_pEntity->GetBonePosition(pEnt, *bone), g_pEntity->GetBonePosition(pEnt, (*(bone + 1))), color);
		}
	}
	
	return 1;
}

int D3D::DrawHealthBar(int x, int y, int h, int hp)
{
	int R = 255 - (hp*2.55);
	int G = hp * 2.55;
	hp = (h - ((h * hp) / 100));

	DrawRect(x - 4, y + hp, 2, h - hp + 1, Color(R, G, 0));
	DrawBorderBox(x - 5, y - 1, 3, h + 2, 1, Color::Black());

	return 1;
}

int D3D::DrawEsp(Ent * pEnt, int entNb)
{
	Vector3 vHead, vFeet, vTop, vBott;
	vHead = g_pEntity->GetBonePosition(pEnt, 8);
	vFeet = g_pEntity->GetBonePosition(pEnt, 1);
	vHead += Vector3(0, 0, 9);

	if (g_pEngine->WorldToScreen(vHead, vTop) && g_pEngine->WorldToScreen(vFeet, vBott))
	{
		float h = vBott.y - vTop.y;
		float w = h / 5.0f;
		
		if (g_pMenu->GetItemStatus(iBox))
		{
			g_pD3D->DrawBorderBoxOut(vTop.x - w, vTop.y, w * 2, h, 1, Color::Green(), Color::Black());
		}

		if (g_pMenu->GetItemStatus(iHealth))
		{
			g_pD3D->DrawHealthBar(vTop.x - w, vTop.y, h, g_pEntity->GetHealth());
		}

		if (g_pMenu->GetItemStatus(iSkeleton))
		{
			g_pD3D->DrawSkeleton(pEnt, Color::Salmon());
		}

		if (g_pMenu->GetItemStatus(iDistance))
		{
			g_pD3D->DrawingText(vTop.x, vTop.y - 10, 10, 10, std::to_wstring(g_pEntity->GetDistanceInMeters(pEnt)) + L" m", Color::White(), pFonts[fDistance]);
		}

		if (g_pMenu->GetItemStatus(iName))
		{
			g_pD3D->DrawingText(vTop.x, vBott.y + 10, 10, 10, charTowstr(g_pEntity->GetName(entNb)), Color::White(), pFonts[fName]);
		}
	}

	return 1;
}

int D3D::PrintBones(Ent * pEnt)
{
	int i = 0;
	Vector3 vBone;
	for (const auto &BoneMat : pEnt->pBoneMatrix->BoneList)
	{
		if (g_pEngine->WorldToScreen(Vector3(BoneMat.m[0][3], BoneMat.m[1][3], BoneMat.m[2][3]), vBone))
		{
			g_pD3D->DrawingText((int)vBone.x - 1, (int)vBone.y - 1, 10, 10, std::to_wstring(i), Color::Green(), pFonts[fBones]);
			i++;
		}
	}

	return 1;
}