#pragma once
#include <Windows.h>
#include <iostream>


class Sig
{
private:
	uintptr_t * m_pSig;
	uintptr_t m_moduleBase;
	size_t m_textSize;
	char* m_pattern;
	char* m_mask;
	int m_index, m_vSize;

public:
	Sig();
	Sig(uintptr_t moduleBase, char* pattern, char* mask, int pattSize, int vSize);
	~Sig();

	int InitDx9Sig();
	uintptr_t SigScan(uintptr_t moduleBase, char* pattern, char* mask, int index, int vSize, uintptr_t offset = 0);
	uintptr_t NetvarsScan(uintptr_t moduleBase, char* pattern, char* mask, int index, int vSize, uintptr_t offset = 0);
	size_t GetBaseOfData(uintptr_t pImageBase);
	int CompareByteArray(PBYTE ByteArray1, PCHAR ByteArray2, PCHAR Mask, size_t Length);
	PBYTE FindSignature(LPVOID BaseAddress, size_t ImageSize, PCHAR Signature, PCHAR Mask);


	uintptr_t * GetSigPtr()
	{
		return m_pSig;
	}

	uintptr_t GetModuleBase()
	{
		return m_moduleBase;
	}

	size_t GetTextSize()
	{
		return m_textSize;
	}

	char* GetPattern()
	{
		return m_pattern;
	}

	char* GetMask()
	{
		return m_mask;
	}

	int GetPatternSize()
	{
		return m_index;
	}

	int GetValueSize()
	{
		return m_vSize;
	}
};