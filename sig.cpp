#include "sig.h"
#include "module.h"
#include "xorstr.h"
#include "macro.h"

// Forward declarations
extern Module* g_pDx9;

// No Sig constructor as Sig class may be initialized before class Module and m_moduleBase be null. Also, xorstr messes with member variables in a constructor
Sig* g_pDx9Sig = new Sig();


//Sig functions
Sig::Sig()
{
}

Sig::Sig(uintptr_t moduleBase, char* pattern, char* mask, int index, int vSize)
	: m_moduleBase(moduleBase), m_pattern(pattern), m_mask(mask), m_index(index), m_vSize(vSize)
{
	m_textSize = GetBaseOfData(m_moduleBase);
	PBYTE sig = (PBYTE)FindSignature((PBYTE)m_moduleBase, m_textSize, (PCHAR)m_pattern, (PCHAR)m_mask);
	memcpy(&m_pSig, (uintptr_t *)(sig + m_index), m_vSize);
}

Sig::~Sig()
{
}

int Sig::InitDx9Sig()
{
	// "shaderapidx9.dll" | "8B 77 1C B9 ?? ?? ?? ??"
	m_moduleBase = g_pDx9->GetImage();
	m_pattern = xorstr("\x8B\x77\x1C\xB9\x00\x00\x00\x00").crypt_get();
	m_mask = xorstr("xxxx????").crypt_get();
	m_index = 4;
	m_vSize = 4;
	m_textSize = GetBaseOfData(m_moduleBase);
	PBYTE sig = (PBYTE)FindSignature((PBYTE)m_moduleBase, m_textSize, (PCHAR)m_pattern, (PCHAR)m_mask);
	memcpy(&m_pSig, (uintptr_t *)(sig + m_index), m_vSize);

	return 1;
}

uintptr_t Sig::SigScan(uintptr_t moduleBase, char* pattern, char* mask, int index, int vSize, uintptr_t offset)
{
	uintptr_t * pSig;
	size_t textSize = GetBaseOfData(moduleBase);
	PBYTE sig = (PBYTE)FindSignature((PBYTE)moduleBase, textSize, (PCHAR)pattern, (PCHAR)mask);
	memcpy(&pSig, (uintptr_t *)(sig + index), vSize);

	if (offset != 0)
	{
		return ((uintptr_t)pSig - moduleBase + offset);
	}
	else
	{
		return ((uintptr_t)pSig - moduleBase);
	}
}

uintptr_t Sig::NetvarsScan(uintptr_t moduleBase, char* pattern, char* mask, int index, int vSize, uintptr_t offset)
{
	uintptr_t * pSig;
	size_t textSize = GetBaseOfData(moduleBase);
	PBYTE sig = (PBYTE)FindSignature((PBYTE)moduleBase, textSize, (PCHAR)pattern, (PCHAR)mask);
	memcpy(&pSig, (uintptr_t *)(sig + index), vSize);

	if (offset != 0)
	{
		return ((uintptr_t)pSig + offset);
	}
	else
	{
		return ((uintptr_t)pSig);
	}
}

// Base of Data = size of (Headers + .text)
size_t Sig::GetBaseOfData(uintptr_t pImageBase)
{
	if (!pImageBase)
		return 0;

	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	PIMAGE_NT_HEADERS Ntheaders = (PIMAGE_NT_HEADERS)((LPBYTE)pImageBase + DosHeader->e_lfanew);

	return (size_t)Ntheaders->OptionalHeader.BaseOfData;
}

int Sig::CompareByteArray(PBYTE ByteArray1, PCHAR ByteArray2, PCHAR Mask, size_t Length)
{
	size_t nextStart = 0;
	char start = ByteArray2[0];
	ROLLED;

	for (size_t i = 0; i < Length; i++)
	{
		ROLLED;
		if (Mask[i] == '?')
		{
			continue;
		}

		ROLLED;
		if (ByteArray1[i] == start)
		{
			nextStart = i;
		}
		ROLLED;
		if (ByteArray1[i] != (BYTE)ByteArray2[i])
		{
			return nextStart;
		}
	}

	return -1;
}

PBYTE Sig::FindSignature(LPVOID BaseAddress, size_t ImageSize, PCHAR Signature, PCHAR Mask)
{
	PBYTE Address = NULL;
	PBYTE Buffer = (PBYTE)BaseAddress;

	ROLLED;
	size_t Length = strlen(Mask);

	ROLLED;
	for (size_t i = 0; i < (ImageSize - Length); i++)
	{
		ROLLED;
		int result = CompareByteArray((Buffer + i), Signature, Mask, Length);

		ROLLED;
		if (result < 0)
		{
			ROLLED;
			Address = (PBYTE)BaseAddress + i;
			break;
		}
		else
		{
			i += result;
		}
	}

	return Address;
}