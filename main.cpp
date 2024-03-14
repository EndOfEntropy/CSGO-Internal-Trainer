#include "include.h"

// Forward declarations
extern Static* g_pStatic;
extern Entity* g_pEntity;
extern Menu* g_pMenu;
extern D3D* g_pD3D;
extern DataHook* g_pDataHook;
extern Morph* g_pMorph;


void CreateConsole()
{
	if (AllocConsole()) {
		freopen("CONOUT$", "w", stdout);
		SetConsoleTitle(L"Debug Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		printf("DLL loaded at %X\n", GetModuleHandle(NULL));
	}
}

int InitGlobalVariables(uintptr_t pImageBase)
{
	Module* dummy = {};

	g_pMorph->MorphModule(pImageBase);
	dummy->InitModules();
	g_pStatic->InitStatic();
	g_pEntity->InitEntity();
	g_pDataHook->InitDataHook();
	g_pD3D->InitD3D();
	g_pMenu->SetMenuPos();	// Called once, res change = bug

	return 1;
}

int __stdcall DllMain(HMODULE hModule, DWORD Reason, LPVOID Reserved)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
	{
		CreateConsole();
		InitGlobalVariables((uintptr_t)hModule);
		//EntityUpdate(g_pCsgoBase->GetImage());
		
		// seems like if the thread does not run the vmt hook doesnt work and a Cx00000005 error occurs. The while loop has to do sth
		/*HANDLE hThread = { 0 };
		hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)EntityUpdate, hModule, 0, nullptr);
		if (hThread)
		{
			CloseHandle(hThread);
		}
		break;*/
		
	}
	case DLL_PROCESS_DETACH:
		//g_pD3D->GetD3DFont()->Release();	// close and release the Font
		//g_pD3D->GetD3DDev()->Release();		// close and release the 3D device
		//FreeConsole();
		//FreeLibraryAndExitThread(hModule, 0); // Stop code execution and free library (no executable crash)
		// Free library (no executable crash).....but still crashes
		
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	
		break;
	}
	return 1;
}

