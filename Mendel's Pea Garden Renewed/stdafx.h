#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN            
#define PI						3.1415926

#pragma region Headers

#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
#include <Psapi.h>

#include <iostream>
#include <fstream>
#include <cmath>
#include <wincodec.h>
#include <algorithm>

#include <d3d11_1.h>
#include <d3d10_1.h>
#include <d3d9.h>
#include <Dcomp.h>
#include <d2d1_3.h>
#include <d2d1_3helper.h>
#include <dxgi.h>
#include <dwrite.h>
#include <Xinput.h>
#include <xaudio2.h>

#pragma endregion Includes headers.

#pragma region Libraries

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "Xinput.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "Dcomp.lib")

#pragma endregion Links libraries.

#pragma warning(disable:4996 4244)

using namespace std;
using namespace D2D1;

// Reduce the length of that ******* union name.
typedef LARGE_INTEGER LINT;

// Template function, releases ppT safely.
// T should be an COM inteface.
// Parameter(s): 
//     ppT (T**): a pointer to the pointer of the interface.
template <
	class T
>
inline void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}


