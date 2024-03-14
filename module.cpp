#pragma once
#include "module.h"
#include "xorstr.h"

Module* g_pClient = new Module();
Module* g_pEngineMod = new Module();
Module* g_pCsgoBase = new Module();
Module* g_pDx9 = new Module();

Module::Module()
{
}

Module::~Module()
{
}

Module::Module(const wchar_t* ImageName)
{
	m_ImageName = wcharTocstr(ImageName);
	m_ImageBase = (uintptr_t)GetModuleW(ImageName);
	m_ImageSize = GetImageSize(m_ImageBase);
	m_TextSize = GetBaseOfData(m_ImageBase);
}

Module::Module(const std::string ImageName)
{
	m_ImageBase = (uintptr_t)GetModuleA(stringTochar(ImageName));
	m_ImageSize = GetImageSize(m_ImageBase);
	m_TextSize = GetBaseOfData(m_ImageBase);
}

Module::Module(const char* ImageName)
{
	m_ImageBase = (uintptr_t)GetModuleA(ImageName);
	m_ImageSize = GetImageSize(m_ImageBase);
	m_TextSize = GetBaseOfData(m_ImageBase);
}

int Module::InitModules()
{
	*g_pClient = (std::string)(xorstr("client_panorama.dll").crypt_get()); // xorstr(L"client_panorama.dll").crypt_get()
	*g_pEngineMod = (std::string)(xorstr("engine.dll").crypt_get());
	*g_pCsgoBase = (std::string)(xorstr("csgo.exe").crypt_get());
	*g_pDx9 = (std::string)(xorstr("shaderapidx9.dll").crypt_get());

	return 1;
}

// Retrieves the address of a loaded module by name (wchar_t*)

uintptr_t __stdcall Module::GetModuleW(const wchar_t* lpModuleName)
{
	struct CLIENT_ID
	{
		HANDLE UniqueProcess;
		HANDLE UniqueThread;
	};

	struct TEB
	{
		NT_TIB NtTib;
		PVOID EnvironmentPointer;
		CLIENT_ID ClientId;
		PVOID ActiveRpcHandle;
		PVOID ThreadLocalStoragePointer;
		struct PEB* ProcessEnvironmentBlock;
		//...
	};

	struct PEB_LDR_DATA
	{
		ULONG Length;
		BOOLEAN Initialized;
		HANDLE SsHandle;
		LIST_ENTRY InLoadOrderModuleList;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
		PVOID EntryInProgress;
		BOOLEAN ShutdownInProgress;
		HANDLE ShutdownThreadId;
	};

	struct PEB
	{
		BOOLEAN InheritedAddressSpace;
		BOOLEAN ReadImageFileExecOptions;
		BOOLEAN BeingDebugged;
		union
		{
			BOOLEAN BitField;
			struct
			{
				BOOLEAN ImageUsesLargePages : 1;
				BOOLEAN IsProtectedProcess : 1;
				BOOLEAN IsImageDynamicallyRelocated : 1;
				BOOLEAN SkipPatchingUser32Forwarders : 1;
				BOOLEAN IsPackagedProcess : 1;
				BOOLEAN IsAppContainer : 1;
				BOOLEAN IsProtectedProcessLight : 1;
				BOOLEAN SpareBits : 1;
			};
		};
		HANDLE Mutant;
		PVOID ImageBaseAddress;
		PEB_LDR_DATA* Ldr;
		//...
	};
	struct UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWCH Buffer;
	};

	struct LDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY InLoadOrderLinks;
		LIST_ENTRY InMemoryOrderLinks;
		union
		{
			LIST_ENTRY InInitializationOrderLinks;
			LIST_ENTRY InProgressLinks;
		};
		PVOID DllBase;
		PVOID EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		//...
	};

	PEB* ProcessEnvironmentBlock = ((PEB*)((TEB*)((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock));
	if (lpModuleName == nullptr)
		return (uintptr_t)(ProcessEnvironmentBlock->ImageBaseAddress);

	PEB_LDR_DATA* Ldr = ProcessEnvironmentBlock->Ldr;

	LIST_ENTRY* ModuleLists[3] = { 0,0,0 };
	ModuleLists[0] = &Ldr->InLoadOrderModuleList;
	ModuleLists[1] = &Ldr->InMemoryOrderModuleList;
	ModuleLists[2] = &Ldr->InInitializationOrderModuleList;
	for (int j = 0; j < 3; j++)
	{
		for (LIST_ENTRY*  pListEntry = ModuleLists[j]->Flink;
			pListEntry != ModuleLists[j];
			pListEntry = pListEntry->Flink)
		{
			LDR_DATA_TABLE_ENTRY* pEntry = (LDR_DATA_TABLE_ENTRY*)((BYTE*)pListEntry - sizeof(LIST_ENTRY) * j); //= CONTAINING_RECORD( pListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks );

			if (_wcsicmp(pEntry->BaseDllName.Buffer, lpModuleName) == 0)
				return (uintptr_t)pEntry->DllBase;

			wchar_t* FileName = GetFileNameFromPath(pEntry->FullDllName.Buffer);
			if (!FileName)
				continue;

			if (_wcsicmp(FileName, lpModuleName) == 0)
				return (uintptr_t)pEntry->DllBase;

			wchar_t FileNameWithoutExtension[256];
			RemoveFileExtension(FileName, FileNameWithoutExtension, 256);

			if (_wcsicmp(FileNameWithoutExtension, lpModuleName) == 0)
				return (uintptr_t)pEntry->DllBase;
		}
	}
	return 0;
}

// Retrieves the address of a loaded module by name (char*)

uintptr_t __stdcall Module::GetModuleA(const char*  lpModuleName)
{
	if (!lpModuleName) return GetModuleW(NULL);

	DWORD ModuleNameLength = (DWORD)strlen(lpModuleName) + 1;

	//allocate buffer for the string on the stack:
	DWORD NewBufferSize = sizeof(wchar_t) * ModuleNameLength;
	wchar_t* W_ModuleName = (wchar_t*)alloca(NewBufferSize);
	for (DWORD i = 0; i < ModuleNameLength; i++)
		W_ModuleName[i] = lpModuleName[i];

	uintptr_t hReturnModule = GetModuleW(W_ModuleName);

	RtlSecureZeroMemory(W_ModuleName, NewBufferSize);

	return hReturnModule;
}

// Retrieves the image size of a loaded module by base address

size_t Module::GetImageSize(uintptr_t pImageBase)
{
	if (!pImageBase)
		return 0;

	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	PIMAGE_NT_HEADERS Ntheaders = (PIMAGE_NT_HEADERS)((LPBYTE)pImageBase + DosHeader->e_lfanew);

	return (size_t)Ntheaders->OptionalHeader.SizeOfImage;
}

// Retrieves the .text size of a loaded module by base address

size_t Module::GetBaseOfData(uintptr_t pImageBase)
{
	if (!pImageBase)
		return 0;

	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	PIMAGE_NT_HEADERS Ntheaders = (PIMAGE_NT_HEADERS)((LPBYTE)pImageBase + DosHeader->e_lfanew);

	return (size_t)Ntheaders->OptionalHeader.BaseOfData;
}

// Utility functions for GetModuleW

wchar_t* Module::GetFileNameFromPath(wchar_t* Path)
{
	wchar_t* LastSlash = NULL;
	for (DWORD i = 0; Path[i] != NULL; i++)
	{
		if (Path[i] == '\\')
			LastSlash = &Path[i + 1];
	}
	return LastSlash;
}

wchar_t* Module::RemoveFileExtension(wchar_t* FullFileName, wchar_t* OutputBuffer, DWORD OutputBufferSize)
{
	wchar_t* LastDot = NULL;
	for (DWORD i = 0; FullFileName[i] != NULL; i++)
		if (FullFileName[i] == '.')
			LastDot = &FullFileName[i];

	for (DWORD j = 0; j < OutputBufferSize; j++)
	{
		OutputBuffer[j] = FullFileName[j];
		if (&FullFileName[j] == LastDot)
		{
			OutputBuffer[j] = NULL;
			break;
		}
	}
	OutputBuffer[OutputBufferSize - 1] = NULL;
	return OutputBuffer;
}