#include <iostream>
#include "static.h"
#include "module.h"
#include "sig.h"
#include "xorstr.h"


Static* g_pStatic = new Static();
// Forward declarations
extern Module* g_pClient;
extern Module* g_pEngineMod;

Static::Static()
{
}

Static::~Static()
{
}

int Static::InitStatic()
{
	Sig* AccessSig = new Sig();
	
	// "client_panorama.dll" | "BB ? ? ? ? 83 FF 01 0F 8C ? ? ? ? 3B F8" (offset = 1st sig ptr - base)
	uintptr_t dwEntityList = AccessSig->SigScan(g_pClient->GetImage(), xorstr("\xBB\x00\x00\x00\x00\x83\xFF\x01\x0F\x8C\x00\x00\x00\x00\x3B\xF8").crypt_get(), 
		xorstr("x????xxxxx????xx").crypt_get(), 1, 4);

	// "engine.dll" | "A1 ? ? ? ? 33 D2 6A 00 6A 00 33 C9 89 B0" (offset = sig ptr - base)
	uintptr_t dwClientState = AccessSig->SigScan(g_pEngineMod->GetImage(), xorstr("\xA1\x00\x00\x00\x00\x33\xD2\x6A\x00\x6A\x00\x33\xC9\x89\xB0").crypt_get(),
		xorstr("x????xxxxxxxxxx").crypt_get(), 1, 4);

	// "client_panorama.dll" | "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9" (offset = sig ptr - base + 0xB0)
	uintptr_t dwViewMatrix = AccessSig->SigScan(g_pClient->GetImage(), xorstr("\x0F\x10\x05\x00\x00\x00\x00\x8D\x85\x00\x00\x00\x00\xB9").crypt_get(),
		xorstr("xxx????xx????x").crypt_get(), 3, 4, 0xB0);

	// "engine.dll" | "68 ? ? ? ? 68 ? ? ? ? FF 50 08 85 C0" (offset = 1st sig ptr - base)
	uintptr_t dwGlobalVars = AccessSig->SigScan(g_pEngineMod->GetImage(), xorstr("\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\xFF\x50\x08\x85\xC0").crypt_get(),
		xorstr("x????x????xxxxx").crypt_get(), 1, 4);

	// "client_panorama.dll" | "8D 34 85 ? ? ? ? 89 15 ? ? ? ? 8B 41 08 8B 48 04 83 F9 FF" extra = 4
	uintptr_t uipLocalPlayer = AccessSig->SigScan(g_pClient->GetImage(), xorstr("\x8D\x34\x85\x00\x00\x00\x00\x89\x15\x00\x00\x00\x00\x8B\x41\x08\x8B\x48\x04\x83\xF9\xFF").crypt_get(),
		xorstr("xxx????xx????xxxxxxxxx").crypt_get(), 3, 4, 0x4);
	
	pCBaseEntList = (CBaseEntityList*)(g_pClient->GetImage() + dwEntityList);
	uClientState = (pClientState*)(g_pEngineMod->GetImage() + dwClientState);
	pWorldToScreenMatrix = (uintptr_t)(g_pClient->GetImage() + dwViewMatrix);
	pGlobal = (uintptr_t)(g_pClient->GetImage() + dwGlobalVars);
	pLocalPlayer = *(uintptr_t*)(g_pClient->GetImage() + uipLocalPlayer);

	delete AccessSig;

	// pLocalPlayer = *(uintptr_t*)(g_pClient->GetImage() + hazedumper::signatures::dwLocalPlayer);
	// pLocalPlayer = *(uintptr_t*)(g_pClient->GetImage() + hazedumper::signatures::dwEntityList) (+10 = EntityList)
	// dwInput = (uintptr_t)(engine.GetImage() + hazedumper::signatures::dwInput);
	// userCMD = (uintptr_t)(dwInput + 0xEC);

	return 1;
}