#pragma once
#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <d3d9.h>
#include <D3dx9.h>
#include "sig.h"


class VirtProt
{
private:
	uintptr_t* m_pAddress;
	size_t m_length;
	uint32_t m_old;

public:
	VirtProt()
	{
	}
	~VirtProt()
	{
	}

	int ChangeProtection(uintptr_t* base, size_t len, uint32_t flags);
};

class DataHook
{
private:
	uintptr_t* m_classbase;
	size_t m_vftsize;
	uintptr_t* m_newvft;
	uintptr_t* m_oldvft;
	uintptr_t* m_oldendscene;
	uintptr_t* m_newendscene;
	uintptr_t m_searchbase;
	bool m_allocated;

public:
	DataHook();
	DataHook(uintptr_t* base);
	~DataHook();
	
	int InitDataHook();
	int PrintMbi(uintptr_t* address);
	size_t VftLength (uintptr_t* vftbl_start);
	uintptr_t* SearchFreePage (uintptr_t* vtable, const size_t vmt_size);
	bool CheckDataProtection(uintptr_t address, const size_t vmt_size);
	bool HookSetup(uintptr_t* base);
	bool IsTableHooked();
	
	int VftHook()
	{
		if (m_newvft != nullptr && (!IsTableHooked()))
		{
			**(uintptr_t***)(m_classbase) = &m_newvft[1];
		}

		return 1;
	}

	int VftUnhook()
	{
		if (m_oldvft != nullptr && IsTableHooked())
		{
			**(uintptr_t***)(m_classbase) = m_oldvft;
		}

		return 1;
	}

	uintptr_t* GetClassBase()
	{
		return m_classbase;
	}

	uintptr_t* GetOldEndScene()
	{
		return m_oldendscene;
	}

	uintptr_t* GetNewEndScene()
	{
		return m_newendscene;
	}
};
