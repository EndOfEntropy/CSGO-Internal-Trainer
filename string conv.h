#pragma once
#include <iostream>
#include <string>

// ref: https://docs.microsoft.com/en-us/cpp/text/how-to-convert-between-various-string-types?view=vs-2019


/*
template <typename T>
void printValue(const T& value)
{
	cout << "Type: " << typeid(value).name() << "\n";
	cout << "Value: " << dec << value << "\n";
	cout << "Address: " << hex << value << "\n";
}
*/

wchar_t* charTowchar(const char* c)
{
	size_t outSize;
	const size_t cSize = strlen(c) + 1;
	wchar_t* wStr = new wchar_t[cSize];
	mbstowcs_s(&outSize, wStr, cSize, c, cSize - 1);

	return wStr;
}

char* wcharTochar(const wchar_t* wc)
{
	size_t outSize;
	const size_t cSize = wcslen(wc) + 1;
	char* cStr = new char[cSize];
	wcstombs_s(&outSize, cStr, cSize, wc, cSize - 1);

	return cStr;
}

wchar_t* wstringTowchar(const std::wstring &ws)
{
	return const_cast<wchar_t*>(ws.c_str());
}

char* stringTochar(const std::string &s)
{
	return const_cast<char*>(s.c_str());
}

std::wstring charTowstr(const char* c)
{
	std::string cStr(c);
	std::wstring wStr(cStr.begin(), cStr.end());

	return wStr;
}

std::string wcharTocstr(const wchar_t* wc)
{
	std::wstring wStr(wc);
	std::string cStr(wStr.begin(), wStr.end());

	return cStr;
}