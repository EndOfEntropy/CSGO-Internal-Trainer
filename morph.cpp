#include "morph.h"
#include "macro.h"
#include "sig.h"

// Morph class
Morph* g_pMorph = new Morph();

// Morph functions

Morph::Morph()
{
}

Morph::Morph(uintptr_t moduleBase) : m_moduleBase(moduleBase)
{
}

Morph::~Morph()
{
}

size_t Morph::GetSizeOfImage(uintptr_t pImageBase)
{
	if (!pImageBase)
		return 0;

	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	PIMAGE_NT_HEADERS Ntheaders = (PIMAGE_NT_HEADERS)((LPBYTE)pImageBase + DosHeader->e_lfanew);

	return (size_t)Ntheaders->OptionalHeader.SizeOfImage;
}

int Morph::MorphModule(uintptr_t pImageBase)
{
	// all the vars we need for the FindSignature() call
	m_moduleBase = pImageBase;
	m_imageSize = GetSizeOfImage(m_moduleBase);

	if (!m_imageSize || m_imageSize > MAXDWORD)
		return 0;

	// the offset from the module base to pLastMatch -> used as: module size - sMatchOffset = range of bytes we still need to scan
	PBYTE pLastMatch = { 0 };
	size_t sMatchOffset = { 0 };
	bool bMorphFinished = false;
	size_t sMatchCount = { 0 };

	while (!bMorphFinished)
	{
		Sig* AccessSig = new Sig();

		// search for the marker we will place in our code later on
		pLastMatch = AccessSig->FindSignature((LPVOID)(m_moduleBase + sMatchOffset), m_imageSize - sMatchOffset, (PCHAR)MARKER_BYTES, (PCHAR)MARKER_MASK);

		// if marker found
		if (pLastMatch != nullptr)
		{
			// call MorphMemory which generates the junk code and writes it to memory
			MorphMemory(pLastMatch, MARKER_SIZE);
			// increment the count and the last match address
			sMatchCount++;
			pLastMatch++;
			// calculate the offset for next search
			sMatchOffset = (size_t)(pLastMatch - m_moduleBase);
		}
		else
		{
			// we found all markers
			bMorphFinished = true;
			// here you can log how many regions got morphed by using sMatchCount
		}

		delete AccessSig;
	}

	return 1;
}

int Morph::MorphMemory(PBYTE pDst, BYTE length)
{
	// get a place for our brand new random opcodes
	PBYTE morphedOpcodes = new BYTE[length];
	BYTE opcodeIt = 0;

	// place the jmp instruction | EB is a short jump, and the following byte is the offset
	morphedOpcodes[opcodeIt] = ASM_OP_JMP_EB;
	opcodeIt++;

	// JMP_Address + 2 + Second_Byte_value = Next_Instruction_Address | inx at address 0x100: (EB 08) = 100 + 2 (EB) + 08 = 0x110
	morphedOpcodes[opcodeIt] = length - ASM_OP_JMP_EB_SIZE;
	opcodeIt++;

	// now fill the rest with random opcodes
	for (; opcodeIt < length; opcodeIt++)
		morphedOpcodes[opcodeIt] = GetRandomNumber(0, MAXBYTE); // 0xFF

	// write the new opcodes to memory | .text is execute/read and .rdata is read-only so you have to use VirtualProtect
	size_t dwOldProtect;
	VirtualProtect(pDst, length, PAGE_EXECUTE_READWRITE, (PDWORD)&dwOldProtect);
	memcpy(pDst, morphedOpcodes, length);
	VirtualProtect(pDst, length, dwOldProtect, (PDWORD)&dwOldProtect);

	// free the used memory again
	delete morphedOpcodes;

	return 1;
}

// Pseudo Random Number Generator
namespace MyRandom
{
	// Initialize our mersenne twister with a random seed based on the clock (once at system startup)
	std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
}

int GetRandomNumber(int min, int max)
{
	std::uniform_int_distribution<> die{ min, max };	// we can create a distribution in any function that needs it | in C++ 17: std::uniform_int_distribution die{ min, max };
	return die(MyRandom::mersenne);						// and then generate a random number from our global generator
}
