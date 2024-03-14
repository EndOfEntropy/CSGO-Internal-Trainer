#pragma once
#include <Windows.h>
#include <random>
#include <ctime>

class Morph
{
private:
	uintptr_t m_moduleBase;
	size_t m_imageSize;


public:
	Morph();
	Morph(uintptr_t moduleBase);
	~Morph();

	size_t GetSizeOfImage(uintptr_t pImageBase);
	int MorphModule(uintptr_t pImageBase);
	int MorphMemory(PBYTE pDst, BYTE length);

	uintptr_t GetModuleBase()
	{
		return m_moduleBase;
	}

	size_t GetImageBase()
	{
		return m_imageSize;
	}

};

// Pseudo Random Number Generator
namespace MyRandom
{
	extern std::mt19937 mersenne;
}

int GetRandomNumber(int min, int max);