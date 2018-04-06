#pragma once

#include "stdafx.h"

// Transform an ANSI string to an Unicode string.
// Parameter(s):
//	   szStr (const char *): a pointer to the ANSI C-style string.
// Returns the UNICODE version of szStr (in WCHAR*).
inline WCHAR* ATOU(const char* szStr) noexcept
{
	int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0);

	if (nLen == 0)
	{
		return NULL;
	}

	wchar_t* pResult = new wchar_t[nLen];

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen);

	return pResult;
}

// Transform an Unicode string to an ANSI string.
// Parameter(s):
//     wszStr (const wchar_t *): a pointer to the UNICODE C-style string.
// Returns the ANSI version of szStr (in char*).
inline char* UTOA(const wchar_t* wszStr) noexcept
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, wszStr, -1, NULL, 0, NULL, NULL);

	if (nLen == 0)
	{
		return NULL;
	}

	char* pResult = new char[nLen];

	WideCharToMultiByte(CP_ACP, 0, wszStr, -1, pResult, nLen, NULL, NULL);
	
	return pResult;
}

// Doing lexical cast to integer.
// Parameter(s):
//     i (const int): the integer to be casted.
// Returns the std::string type that contains the value of i, in decimal.
inline string ITOA(const int i) noexcept
{
	string out;
	int j = i;
	while (j)
	{
		out += char(0x30 + j % 10);
		j /= 10;
	}

	reverse(out.begin(), out.end());

	return out;
}

// Ummmm... Possibly ATOU with a longer name???
// I don't want to give any explanation.
__declspec(deprecated) inline WCHAR* ANSIToUnicode(const char* sz) noexcept
{
	DWORD dwLength = MultiByteToWideChar(CP_ACP, 0, sz, -1, NULL, 0);

	WCHAR* pwsz = new WCHAR[dwLength];

	if (!pwsz)
	{
		delete[] pwsz;
	}

	MultiByteToWideChar(CP_ACP, 0, sz, -1, pwsz, dwLength);

	return pwsz;
}

// Ummmm... Possibly UTOA with a longer name???
// I don't want to give any explanation.
__declspec(deprecated) inline char* UnicodeToANSI(const WCHAR* wsz) noexcept
{
	DWORD dwLength = WideCharToMultiByte(CP_OEMCP, NULL, wsz, -1, NULL, 0, NULL, FALSE);

	char* psz = new char[dwLength];

	if (!psz)
	{
		delete[] psz;
	}

	WideCharToMultiByte(CP_OEMCP, NULL, wsz, -1, psz, dwLength, NULL, FALSE);

	return psz;
}

// Doing lexical cast to a double value, return format as follows: "XX.XX".
// Parameter(s):
//     d (const double): the double value to be casted.
// Returns the UNICODE C-style string of the tens, ones, tenths and hundredths of d.
inline WCHAR* DoubleToString(const double d) noexcept
{
	double dd = d * 100.0f;

	int a = dd;

	char sz[6] = "  .  ";

	sz[4] = a % 10 + 0x30;
	sz[3] = (a % 100) / 10 + 0x30;
	sz[1] = (a % 1000) / 100 + 0x30;
	sz[0] = (a % 10000) / 1000 + 0x30;

	return ATOU(sz);
}

// Function that calculates POWER!
// Parameter(s):
//     x (unsigned long long): the base of the power.
//     y (unsigned int): the exponent of the power.
// Returns the yth power of x, in unsigned long long.
constexpr unsigned long long Pow(unsigned long long x, UINT y) noexcept
{
	unsigned long long ret = 1;

	while (y)
	{
		if (y & 1)ret *= x;
		x *= x;
		y >>= 1;
	}

	return ret;
}
