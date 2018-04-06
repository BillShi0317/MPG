#pragma once

#include "resource.h"
#include "audio.h"
#include "graphics.h"
#include "input.h"

#pragma region Macros

#define RANDOM_SEED 19260817
#define GROW_FRAME 600
#define STARTUP 0
#define OUTSIDE 1
#define INSIDE 2
#define ENDING 3
#define SETTINGS 4
#define ICONS 5
#define TREE 6

#pragma endregion

#pragma region Global Variables

DOUBLE					g_dFPS;
bool					g_bGeneticMutation = false;
UINT					g_nStat;										// A global unsigned integer value, indicates current status.
UINT					g_nSeedCache = 0;
UINT					g_nUsedSeedCache = 0;
UINT					g_nPlantCache = 0;
UINT					g_nSceneChangeCounter = 0;
UINT					g_nLastX;
UINT					g_nLastY;
RECT					g_rcStartButton = { 400, 400, 640, 500 };
RECT					g_rcEndButton = { 400, 540, 640, 640 };

#pragma endregion

namespace MPG
{
	typedef enum SEED_TYPE
	{
		SEED_TYPE_GREEN_ANGULAR,
		SEED_TYPE_YELLOW_ANGULAR,
		SEED_TYPE_GREEN_ROUND,
		SEED_TYPE_YELLOW_ROUND,
		SEED_TYPE_UNUSED = 0x80000000
	}*PSEED_TYPE;

	typedef enum PLOT_TYPE
	{
		PLOT_TYPE_PLANTED_TOP_WHITE = 0x00000000,
		PLOT_TYPE_PLANTED_MID_WHITE = 0x00010000,
		PLOT_TYPE_PLANTED_TOP_PURPLE = 0x00020000,
		PLOT_TYPE_PLANTED_MID_PURPLE = 0x00030000,
		PLOT_TYPE_UNPLANTED = 0x80000000
	}*PPLOT_TYPE;

	typedef enum PLOT_STATUS
	{
		PLOT_STATUS_PLANTED_SPROUT,
		PLOT_STATUS_PLANTED_MATURE,
		PLOT_STATUS_PLANTED_POLINATED,
		PLOT_STATUS_PLANTED_HARVESTABLE,
		PLOT_STATUS_UNPLANTED = 0x80000000
	}*PPLOT_STATUS;

	typedef class _SEED
	{
	public:
		_SEED();
		~_SEED();

		void CreateSeed(UINT, UINT, UINT);
		UINT GetID();
		UINT GetParentID(int);
		UINT GetGrandparentID(int, int);
		UINT GetGamete();
		SEED_TYPE GetSeedType();
		__declspec(deprecated) void SetID(UINT);
		void SetGamete(UINT);

	private:
		UINT nID;
		UINT nGamete;
		UINT nParentID[2] = { 0x80000000, 0x80000000 };
		UINT nGParentID[2][2] = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
		SEED_TYPE st;
	}SEED, *PSEED;

	typedef class _PLOT
	{
	public:
		_PLOT();
		~_PLOT();

		void Plant(SEED seed, UINT nFrameID);
		void Grow(UINT nFrameID);
		void Harvest();

		void operator*(_PLOT& plot);
		
	private:
		UINT nID;
		UINT nSeedID;
		PLOT_TYPE plType;
		PLOT_STATUS plStatus;
		UINT nPlantFrame = 0;
		SEED seed[4];
		UINT nPolPlotID;
	}PLOT, *PPLOT;

	typedef enum MPG_SCENE
	{
		MPG_SCENE_STARTUP,
		MPG_SCENE_SETTINGS,
		MPG_SCENE_OUTSIDE_NOMENU,
		MPG_SCENE_OUTSIDE_MENU,
		MPG_SCENE_INSIDE_NOMENU,
		MPG_SCENE_INSIDE_MENU,
		MPG_SCENE_ENDING
	}*PMPG_SCENE;
}

MPG::PSEED pSeeds = new MPG::SEED[1024];
MPG::PPLOT pPlots = new MPG::PLOT[16];
MPG::MPG_SCENE g_sceneCurr = MPG::MPG_SCENE_STARTUP, g_sceneNext = MPG::MPG_SCENE_STARTUP;

inline BOOL IsPointInRect(UINT nX, UINT nY, RECT rc) noexcept
{
	if (rc.left <= nX && nX <= rc.right)
		if (rc.top <= nY && nY <= rc.bottom)
			return TRUE;

	return FALSE;
}

D2D1_RECT_F CastRect(RECT rc)
{
	return RectF(rc.left, rc.top, rc.right, rc.bottom);
}

void ChangeScene(MPG::MPG_SCENE scene)
{
	if (!g_nSceneChangeCounter)
	{
		g_sceneNext = scene;
		g_nSceneChangeCounter = 120;
	}
}

HRESULT PaintHandler(MPG::MPG_SCENE scene, UINT nX, UINT nY, UINT nScale, HWND hWnd, RECT rc, ID2D1DeviceContext* pDC, IDXGISwapChain1* pSC, ID2D1Bitmap** pBmp, IDWriteTextFormat* pFormat, ID2D1SolidColorBrush* pBrush, IXAudio2SourceVoice** ppSourceVoice)
{
	bool bUserQuit = false;

	pDC->BeginDraw();
	pDC->Clear(ColorF(1.0f, 1.0f, 1.0f, 1.0f));
	
	switch (scene)
	{
	case MPG::MPG_SCENE_STARTUP:
	{
		pDC->DrawBitmap(
			pBmp[0]
		);

		SHORT sLBState = GetAsyncKeyState(VK_LBUTTON);

		if (!IsPointInRect(nX, nY, g_rcStartButton))
		{
			pDC->DrawBitmap(
				pBmp[5],
				RectF(100.0f * nScale, 100.0f * nScale, 160.0f * nScale, 125.0f * nScale),
				1.0f,
				D2D1_INTERPOLATION_MODE_LINEAR,
				RectF(0.0f, 0.0f, 480.0f, 200.0f)
			);
		}
		else if (sLBState & 0x8000)
		{
			pDC->DrawBitmap(
				pBmp[5],
				RectF(100.0f * nScale, 100.0f * nScale, 160.0f * nScale, 125.0f * nScale),
				1.0f,
				D2D1_INTERPOLATION_MODE_LINEAR,
				RectF(480.0f, 0.0f, 960.0f, 200.0f)
			);

			if (sLBState & 0x0001 && hWnd == GetForegroundWindow())
				ChangeScene(MPG::MPG_SCENE_OUTSIDE_NOMENU);
		}
		else
		{
			pDC->DrawBitmap(
				pBmp[5],
				RectF(100.0f * nScale, 100.0f * nScale, 160.0f * nScale, 125.0f * nScale),
				1.0f,
				D2D1_INTERPOLATION_MODE_LINEAR,
				RectF(960.0f, 0.0f, 1440.0f, 200.0f)
			);
		}
		if (!IsPointInRect(nX, nY, g_rcEndButton))
		{
			pDC->DrawBitmap(
				pBmp[5],
				RectF(100.0f * nScale, 135.0f * nScale, 160.0f * nScale, 160.0f * nScale),
				1.0f,
				D2D1_INTERPOLATION_MODE_LINEAR,
				RectF(0.0f, 200.0f, 480.0f, 400.0f)
			);
		}
		else if (sLBState & 0x8000)
		{
			pDC->DrawBitmap(
				pBmp[5],
				RectF(100.0f * nScale, 135.0f * nScale, 160.0f * nScale, 160.0f * nScale),
				1.0f,
				D2D1_INTERPOLATION_MODE_LINEAR,
				RectF(480.0f, 200.0f, 960.0f, 400.0f)
			);

			if (sLBState & 0x0001 && hWnd == GetForegroundWindow())
			{
				bUserQuit = true;
			}
		}
		else
		{
			pDC->DrawBitmap(
				pBmp[5],
				RectF(100.0f * nScale, 135.0f * nScale, 160.0f * nScale, 160.0f * nScale),
				1.0f,
				D2D1_INTERPOLATION_MODE_LINEAR,
				RectF(960.0f, 200.0f, 1440.0f, 400.0f)
			);
		}
	}
	break;
	case MPG::MPG_SCENE_SETTINGS:
	{
		pDC->DrawBitmap(
			pBmp[4]
		);
	}
	break;
	case MPG::MPG_SCENE_OUTSIDE_NOMENU:
	{
		pDC->DrawBitmap(
			pBmp[1]
		);

		if (!(g_nStat & 0x00000010))
		{
			pDC->DrawBitmap(
				pBmp[5],
				RectF(200.0f, 200.0f, nX, nY),
				1.0f,
				D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				RectF(0.0f, 0.0f, 1280.0f, 960.0f)
			);

			g_nLastX = nX;
			g_nLastY = nY;
		}
		else
		{
			pDC->DrawBitmap(
				pBmp[5],
				RectF(200.0f, 200.0f, g_nLastX, g_nLastY),
				1.0f,
				D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				RectF(0.0f, 0.0f, 1280.0f, 960.0f)
			);
		}

		if (GetAsyncKeyState(VK_TAB) & 0x0001 && hWnd == GetForegroundWindow())
			ChangeScene(MPG::MPG_SCENE_STARTUP);

		short sB = GetAsyncKeyState(VK_LBUTTON);

		if (sB & 0x0001 && hWnd == GetForegroundWindow())
			g_nStat ^= 0x00000010;
	}
	break;
	case MPG::MPG_SCENE_OUTSIDE_MENU:
	{

	}
	break;
	case MPG::MPG_SCENE_INSIDE_NOMENU:
	{
		pDC->DrawBitmap(
			pBmp[2]
		);
	}
	break;
	case MPG::MPG_SCENE_INSIDE_MENU:
	{

	}
	break;
	case MPG::MPG_SCENE_ENDING:
	{
		pDC->DrawBitmap(
			pBmp[3]
		);
	}
	break;
	}

	ID2D1SolidColorBrush* pGrayBrush;

	pDC->CreateSolidColorBrush(
		ColorF(0.0f, 0.0f, 0.0f, 1.0f - (abs((double)g_nSceneChangeCounter - 60) / 60.0f)),
		&pGrayBrush
	);

	pDC->FillRectangle(
		RectF(0.0f, 0.0f, 320.0f * nScale, 240.0f * nScale),
		pGrayBrush
	);

	pDC->DrawTextW(
		DoubleToString(g_dFPS),
		6,
		pFormat,
		RectF(0.0f, 0.0f, rc.right - rc.left, rc.bottom - rc.top),
		pBrush
	);

	HRESULT hr = pDC->EndDraw();

	pSC->Present(1, 0);

	if (g_nSceneChangeCounter > 0)
	{
		g_nSceneChangeCounter--;

		if (g_nSceneChangeCounter == 60)
		{
			g_sceneCurr = g_sceneNext;
			ppSourceVoice[5]->Start(0);

		}
	}
	
	SafeRelease(&pGrayBrush);
	
	if (bUserQuit)
		DestroyWindow(hWnd);

	return hr;
}



#pragma region Definitions

inline MPG::_SEED::_SEED()
{
	nID = g_nSeedCache++;
	nGamete = 0x80000000;	// It's unused.
	st = SEED_TYPE_UNUSED;
}

inline MPG::_SEED::~_SEED()
{

}

inline void MPG::_SEED::CreateSeed(UINT nG, UINT nPID0, UINT nPID1)
{
	nGamete = nG;
	nParentID[0] = nPID0;
	nParentID[1] = nPID1;
	nGParentID[0][0] = (pSeeds + nPID0)->nParentID[0];
	nGParentID[0][1] = (pSeeds + nPID0)->nParentID[1];
	nGParentID[1][0] = (pSeeds + nPID1)->nParentID[0];
	nGParentID[1][1] = (pSeeds + nPID1)->nParentID[1];
	st = (SEED_TYPE)(((nGamete & 0x0000000C) >> 2) | (nGamete & 0x00000003));
	g_nUsedSeedCache++;
}

inline UINT MPG::_SEED::GetID()
{
	return nID;
}

inline UINT MPG::_SEED::GetParentID(int nIndex)
{
	if (nIndex == 0 || nIndex == 1)
		return nParentID[nIndex];

	return 0x80000000;
}

inline UINT MPG::_SEED::GetGrandparentID(int nIndex0, int nIndex1)
{
	if ((nIndex0 == 0 || nIndex0 == 1) && (nIndex1 == 0 || nIndex1 == 1))
		return nGParentID[nIndex0][nIndex1];

	return 0x80000000;
}

inline UINT MPG::_SEED::GetGamete()
{
	return nGamete;
}

__forceinline MPG::SEED_TYPE MPG::_SEED::GetSeedType()
{
	return st;
}

__declspec(deprecated) inline void MPG::_SEED::SetID(UINT ID)
{
	nID = ID;
	g_nSeedCache = ID;
}

inline void MPG::_SEED::SetGamete(UINT nG)
{
	nGamete = nG;
	st = (SEED_TYPE)(((nGamete & 0x0000000C) >> 2) | (nGamete & 0x00000003));
}

inline MPG::_PLOT::_PLOT()
{
	nID = g_nPlantCache++;
	nSeedID = 0x80000000;
	plType = PLOT_TYPE_UNPLANTED;
	plStatus = PLOT_STATUS_UNPLANTED;
	nPolPlotID = 0x80000000;
}

inline MPG::_PLOT::~_PLOT()
{

}

inline void MPG::_PLOT::Plant(SEED seed, UINT nFrameID)
{
	nSeedID = seed.GetID();
	plStatus = PLOT_STATUS_PLANTED_SPROUT;
	plType = (PLOT_TYPE)((seed.GetGamete() & 0x00030000) | ((seed.GetGamete() & 0x000C0000) >> 2));
	nPlantFrame = nFrameID;
}

inline void MPG::_PLOT::Grow(UINT nFrameID)
{
	if (plStatus != PLOT_STATUS_UNPLANTED && nFrameID == 0xFFFFFFFF && plStatus != PLOT_STATUS_PLANTED_HARVESTABLE)
	{
		plStatus = (PLOT_STATUS)((int)plStatus + 1);
	}
	else if (plStatus != PLOT_STATUS_UNPLANTED && nFrameID - nPlantFrame >= GROW_FRAME && plStatus != PLOT_STATUS_PLANTED_HARVESTABLE)
	{
		plStatus = (PLOT_STATUS)((int)(plStatus)+1);
		nPlantFrame = nFrameID;
	}
}

inline void MPG::_PLOT::Harvest()
{
	if (plStatus == PLOT_STATUS_PLANTED_HARVESTABLE)
	{
		for (int i = 0; i < 4; i++)
		{
			SEED &ret = *(pSeeds + g_nUsedSeedCache);
			ret.CreateSeed(seed[0].GetGamete(), seed[0].GetParentID(0), seed[0].GetParentID(1));
		}
	}
	
	pPlots[this->nPolPlotID].plStatus = PLOT_STATUS_UNPLANTED;
	pPlots[this->nPolPlotID].plType = PLOT_TYPE_UNPLANTED;
	pPlots[this->nPolPlotID].nSeedID = 0x80000000;
	pPlots[this->nPolPlotID].nPlantFrame = 0;
	pPlots[this->nPolPlotID].nPolPlotID = 0x80000000;
	for (int i = 0; i < 4; i++)
	{
		pPlots[this->nPolPlotID].seed[i].CreateSeed(0x80000000, 0x80000000, 0x80000000);
	}
	this->plStatus = PLOT_STATUS_UNPLANTED;
	this->plType = PLOT_TYPE_UNPLANTED;
	this->nSeedID = 0x80000000;
	this->nPlantFrame = 0;
	this->nPolPlotID = 0x80000000;
	for (int i = 0; i < 4; i++)
	{
		this->seed[i].CreateSeed(0x80000000, 0x80000000, 0x80000000);
	}
}

inline void MPG::_PLOT::operator*(_PLOT& plot)
{
	if (this->plStatus == PLOT_STATUS_PLANTED_MATURE && plot.plStatus == PLOT_STATUS_PLANTED_MATURE)
	{
		UINT gamete0 = (pSeeds + this->nSeedID)->GetGamete();
		UINT gamete1 = (pSeeds + plot.nSeedID)->GetGamete();
		UINT nGamete[4];

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if ((gamete0 & (1 << j)) == (gamete1 & (1 << j)))
				{
					nGamete[i] |= gamete0 & (1 << j);
				}
				else
				{
					nGamete[i] |= (1 << j) * (rand() % 2);
				}
				if ((gamete0 & (1 << (j + 16))) == (gamete1 & (1 << (j + 16))))
				{
					nGamete[i] |= gamete0 & (1 << (j + 16));
				}
				else
				{
					nGamete[i] |= (1 << (j + 16)) * (rand() % 2);
				}
			}
			seed[i].CreateSeed(nGamete[i], this->nSeedID, plot.nSeedID);
			plot.seed[i].CreateSeed(nGamete[i], this->nSeedID, plot.nSeedID);
		}
		this->nPolPlotID = plot.nID;
		plot.nPolPlotID = this->nID;
		this->Grow(0xFFFFFFFF);
		plot.Grow(0xFFFFFFFF);
	}
}

#pragma endregion What is this??


