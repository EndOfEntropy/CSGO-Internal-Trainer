#pragma once
#include <string>
#include <Windows.h>
#include "string conv.h"


class Module
{
private:

	uintptr_t			m_ImageBase;
	size_t				m_ImageSize;
	size_t				m_TextSize;
	std::string			m_ImageName;
	//string			m_ImagePath;

public:

	Module();
	~Module();
	Module(const wchar_t* ImageName);
	Module(const std::string ImageName);
	Module(const char* ImageName);

	uintptr_t GetImage()
	{
		return m_ImageBase;
	}

	size_t GetImageSize()
	{
		return m_ImageSize;
	}

	size_t GetTextSize()
	{
		return m_TextSize;
	}

	const std::string& GetName()
	{
		return m_ImageName;
	}

	/*const std::string&	GetPath() const
	{
		return m_ImagePath;
	}*/

	int InitModules();

	uintptr_t __stdcall GetModuleA(const char*  lpModuleName);
	uintptr_t __stdcall GetModuleW(const wchar_t* lpModuleName);
	size_t GetImageSize(uintptr_t pImageBase);
	size_t GetBaseOfData(uintptr_t pImageBase);

	wchar_t* GetFileNameFromPath(wchar_t* Path);
	wchar_t* RemoveFileExtension(wchar_t* FullFileName, wchar_t* OutputBuffer, DWORD OutputBufferSize);

};