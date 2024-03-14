#include "datahook.h"
#include "module.h"

// VirtProt class
VirtProt* g_pVirtProt = new VirtProt();
DataHook* g_pDataHook = new DataHook();

// Forward declarations
extern Sig* g_pDx9Sig;
extern Module* g_pDx9;

// VirtProt functions
int VirtProt::ChangeProtection(uintptr_t* base, size_t len, uint32_t flags)
{
	m_pAddress = base;
	m_length = len;

	if (!VirtualProtect((LPVOID)base, len, flags, (PDWORD)&m_old))
	{
		//cout << "VirtualProtect failed!" << "\n";
		return 0;
	}

	return 1;
}

// DataHook functions
DataHook::DataHook()
	: m_classbase(nullptr), m_vftsize(0), m_newvft(nullptr), m_oldvft(nullptr), m_oldendscene(nullptr), m_newendscene(nullptr), m_searchbase(0), m_allocated(0)
{
}

DataHook::DataHook(uintptr_t* base)
	: m_classbase(base), m_vftsize(0), m_newvft(nullptr), m_oldvft(nullptr), m_oldendscene(nullptr), m_newendscene(nullptr), m_searchbase(0), m_allocated(0)
{
}

DataHook::~DataHook()
{
	VftUnhook();
	if (m_allocated)
		delete[] m_newvft;
}

int DataHook::InitDataHook()
{
	g_pDx9Sig->InitDx9Sig();
	uintptr_t* pd3ddev = *(uintptr_t**)(g_pDx9Sig->GetSigPtr()); // To remove
	uintptr_t* VTable = *(uintptr_t**)(pd3ddev);
	//g_pDataHook->PrintMbi(VTable);

	g_pDataHook->HookSetup(g_pDx9Sig->GetSigPtr());

	return 1;
}

int DataHook::PrintMbi(uintptr_t* address)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if (VirtualQuery((LPCVOID)address, &mbi, sizeof(mbi)))
	{
		/*
		cout << "BaseAddress: 0x" << hex << mbi.BaseAddress << "\n";
		cout << "AllocationBase: 0x" << hex << mbi.AllocationBase << "\n";
		cout << "AllocationProtect: 0x" << hex << mbi.AllocationProtect << "\n";
		cout << "RegionSize: 0x" << hex << mbi.RegionSize << "\n";
		cout << "State: 0x" << hex << mbi.State << "\n";
		cout << "Protect: 0x" << hex << mbi.Protect << "\n";
		cout << "Type: 0x" << hex << mbi.Type << "\n";
		*/

		return 1;
	}

	return 0;
}

size_t DataHook::VftLength(uintptr_t* vftbl_start)
{
	MEMORY_BASIC_INFORMATION memInfo = { NULL };
	size_t m_nSize = 0;
	do {
		VirtualQuery((LPCVOID)(vftbl_start[m_nSize]), &memInfo, sizeof(memInfo));
		m_nSize++;

	} while (memInfo.Protect == PAGE_EXECUTE_READ || memInfo.Protect == PAGE_EXECUTE_READWRITE);

	return m_nSize - 1;
}

uintptr_t* DataHook::SearchFreePage(uintptr_t* vtable, const size_t vmt_size)
{
	MEMORY_BASIC_INFORMATION vMbi = { 0 };

	if (!VirtualQuery((LPCVOID)vtable, &vMbi, sizeof(vMbi)))
		return 0;

	// Based on vtable mbi, search from the BaseAddress->RegionEnd
	m_searchbase = (uintptr_t)vMbi.BaseAddress;
	uintptr_t regionStart = (uintptr_t)vMbi.BaseAddress;
	uintptr_t regionEnd = (uintptr_t)vMbi.BaseAddress + vMbi.RegionSize;

	// debug this function to understand the ifs. Make it sig scan proof.
	for (uintptr_t currAddress = regionStart; currAddress < regionEnd; currAddress += sizeof(uintptr_t))
	{
		if (*(uintptr_t*)currAddress == 0 && CheckDataProtection(currAddress, vmt_size))
		{
			uintptr_t ptr_count = { 0 };
			bool zeroMem = 1;

			for (; ptr_count < vmt_size && zeroMem; ptr_count += sizeof(uintptr_t))
			{
				if (*(uintptr_t*)(currAddress + ptr_count) != 0)
					zeroMem = 0;
			}

			if (zeroMem && ptr_count >= vmt_size)
				return (uintptr_t*)currAddress;
		}
	}
	return 0;
}

// Checks for various memory page settings, see bitwise operator (&)
bool DataHook::CheckDataProtection(uintptr_t address, const size_t vmt_size)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	const unsigned long DataProtection = (PAGE_EXECUTE_READWRITE | PAGE_READWRITE);

	if (VirtualQuery((LPCVOID)address, &mbi, sizeof(mbi)) != sizeof(mbi))
		return 0;

	if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS && !(mbi.Protect & PAGE_GUARD))
	{
		if (mbi.AllocationBase && mbi.BaseAddress && (mbi.Protect & DataProtection) && mbi.RegionSize >= vmt_size)
		{
			return 1;
		}
	}
	return 0;
};

// Includes class members initialization
bool DataHook::HookSetup(uintptr_t* base)
{
	if (base == nullptr)
		return 0;

	m_classbase = base;

	// Get oldvtf and vtfsize
	m_oldvft = **(uintptr_t***)(m_classbase);
	m_vftsize = VftLength(m_oldvft) * sizeof(uintptr_t);

	if (!m_vftsize)
		return 0;
		
	// Search for free page and copy vft to it (m_newvft[0] = Free page ptr; m_newvft[1] = vtf beginning
	m_newvft = SearchFreePage(m_oldvft, m_vftsize + sizeof(uintptr_t));
	if (m_newvft == nullptr)
		return 0;
	memset(m_newvft, 0, m_vftsize + sizeof(uintptr_t));
	memcpy(&m_newvft[1], m_oldvft, m_vftsize);
	m_newvft[0] = m_oldvft[-1];
	m_newendscene = &m_newvft[42];
	m_oldendscene = &m_oldvft[42];
	m_allocated = 1;

	// No need to VirtualProtect as that region is already RW
	// auto guard = g_pVirtProt->ChangeProtection(*(uintptr_t**)m_classbase, sizeof(uintptr_t), PAGE_READWRITE);

	// Place hook, oldvft points to newvft (use oldvft ptr to unhook as its unchanged)
	**(uintptr_t***)(m_classbase) = &m_newvft[1];
	if (**(uintptr_t***)(m_classbase) != &m_newvft[1])
		return 0;

	return 1;
}

bool DataHook::IsTableHooked()
{
	if (m_classbase == nullptr || m_newvft == nullptr)
		return 0;

	if (**(uintptr_t***)(m_classbase) == &m_newvft[1])
		return 1;

	return 0;
}