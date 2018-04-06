#include "stdafx.h"
#include "Mendel's Pea Garden Renewed.h"

#pragma hdrstop

#define MAX_LOADSTRING 255

using namespace MPG;

#pragma region Interface declaration

IDXGIAdapter*			pDXGIAdapter = NULL;
IDXGIDevice1*			pDXGIDevice = NULL;
IDXGIFactory2*			pDXGIFactory = NULL;
IDXGISwapChain1*		pDXGISC = NULL;
IDXGISurface2*			pDXGISurfaceBackBuffer = NULL;

ID3D11Device*			pD3DDevice = NULL;
ID3D11DeviceContext*	pD3DDC = NULL;

IDCompositionDevice*	pDCompDevice = NULL;
IDCompositionTarget*	pDCompTarget = NULL;
IDCompositionVisual*	pDCompVisual = NULL;
IDCompositionSurface*	pDCompSurface = NULL;

ID2D1Factory1*			pD2DFactory = NULL;
ID2D1Device*			pD2DDevice = NULL;
ID2D1DeviceContext*		pD2DDC = NULL;
ID2D1Bitmap1*			pD2DTargetBitmap = NULL;
ID2D1Effect*			pD2DEffect = NULL;
ID2D1SolidColorBrush*	pBrush = NULL;									// A pointer to the Direct2D solid color brush interface.
ID2D1Bitmap*			pBitmap[IMAGE_FILE_COUNT];						// Pointers to the Direct2D bitmap interfaces.
ID2D1BitmapBrush*		pBitmapBrush[IMAGE_FILE_COUNT];					// Pointers to the Direct2D bitmap brush interfaces.

IWICImagingFactory2*	pWICFactory = NULL;								// A pointer to the Windows Imaging Component factory interface.
IWICBitmap*				pWICBitmap[IMAGE_FILE_COUNT];

IDWriteFactory*			pDWFactory = NULL;								// A pointer to the DirectWrite factory.
IDWriteTextFormat*		pFormat = NULL;									// A pointer to the DirectWrite text format.

IXAudio2*				pEngine = NULL;									// A pointer to the XAudio2 main engine interface.
IXAudio2MasteringVoice* pMasterVoice = NULL;							// A pointer to the XAudio2 mastering voice interface.
IXAudio2SourceVoice*	pSourceVoice[AUDIO_FILE_COUNT];					// Pointers to the XAudio2 source voice interfaces.
IXAudio2VoiceCallback*	pSECallback[2];

#pragma endregion Declares Component Object Model interfaces we used.

#pragma region Global variable declaration

D3D_FEATURE_LEVEL		g_FeatureLevel;
CONTROLLER				g_Ctrl[XUSER_MAX_COUNT];						// Game controllers.
HINSTANCE				g_hInstance;									// A global handle of this program's instance.
HRESULT					g_hr;											// A global handle of Component Object Model function results.
HANDLE					g_hConsole;										// A global handle of debug console.
WCHAR					g_szTitle[MAX_LOADSTRING];						// A global wide string which contains the window title.
WCHAR					g_szWindowClass[MAX_LOADSTRING];				// A global wide string which contains the window class.
WCHAR					g_szAudioFilePath[AUDIO_FILE_COUNT][MAX_PATH];	// Global wide strings which contain the absolute path of audio files.
WCHAR					g_szImageFilePath[IMAGE_FILE_COUNT][MAX_PATH];	// Global wide strings which contain the absolute path of image files.
HWND					g_hWnd;											// A global handle of the main window.
HWND					g_hDlg;											// A global handle of the resizing dialog window.
UINT					g_nCreationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
UINT					g_nSize;										// A global unsigned integer which describes the scaling factor, can be 2, 3 or 4.
UINT					g_nBGMID;										// A global unsigned integer which describes the ID of the track which is playing, it's only for testing and debugging uses, should be deleted in the final version.
UINT					g_nCxFrame;
UINT					g_nCyFrame;
UINT					g_nCyCaption;
UINT					g_nReserved[2];									// Global unsigned integer values, it's only for debug uses.
BOOL					g_bRTUpdate;									// A global boolean value, is TRUE if and only if the render target is changed.
BOOL					g_bConnected[XUSER_MAX_COUNT];					// Global boolean values, is TRUE if the correspond ID has a device connected.

#pragma endregion Declares global variables.

#pragma region Function predeclaration

LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);			// Main window procedure function.
INT_PTR CALLBACK		DlgProc(HWND, UINT, WPARAM, LPARAM);			// Dialog window procedure function.
inline WORD				MyRegisterClass(HINSTANCE);						// Registers the window class in Windows.
inline BOOL				InitInstance(HINSTANCE, int);					// Initializes the main instance and window.
inline BOOL				InitDialog(HINSTANCE, int);						// Initializes the dialog window.
inline BOOL				InitString(HINSTANCE);							// Initializes global strings.
constexpr BOOL			InitVariable() noexcept;						// Initializes some global variables.
inline HRESULT			CreateGraphicsResources(HWND);					// Creates DirectX graphics resources.
inline HRESULT			CreateBitmapResources();						// Creates Direct2D bitmaps.
inline HRESULT			CreateXA2Resources();							// Creates XAudio2 resources.
inline HRESULT			CreateVoiceResources();							// Creates XAudio2 source voices.
inline double			FPS() noexcept;									// Gives the frames rendered in the last second.

#pragma endregion Pre-declares functions in this piece.



#pragma region Entry function

																		// The entry point of the program.
int APIENTRY wWinMain(
	_In_ HINSTANCE		hInstance,										// The instance.
	_In_opt_ HINSTANCE	hPrevInstance,									// The previous instance.
	_In_ LPWSTR			lpCmdLine,										// The command line.
	_In_ int			nCmdShow										// Integer, describes show status.
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	InitString(hInstance);
	InitVariable();
	MyRegisterClass(hInstance);

	if (!InitDialog(hInstance, nCmdShow))
	{
		return FALSE;
	}

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;

	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return (int)msg.wParam;
}

#pragma endregion Declares the entry function.



#pragma region Procedure functions

// Main window procedure function.
LRESULT CALLBACK WndProc(
	_In_ HWND			hWnd,											// A handle of the window.
	_In_ UINT			message,										// The message.
	_In_ WPARAM			wParam,											// Additional parameter.
	_In_ LPARAM			lParam											// Additional parameter.
)
{
	switch (message)
	{
	case WM_CREATE:
	{
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);

		g_hr = CreateXA2Resources();

		if (FAILED(g_hr))
		{
			PostQuitMessage((int)g_hr);
		}

		g_hr = CreateVoiceResources();

		if (FAILED(g_hr))
		{
			PostQuitMessage((int)g_hr);
		}
		
		pSourceVoice[g_nBGMID]->Start(0);
	}
	break;
	case WM_PAINT:
	{
		g_nReserved[0]++;

		POINT ptCursor;
		RECT rc;
		RECT rcWnd;

		GetCursorPos(&ptCursor);
		GetWindowRect(g_hWnd, &rcWnd);
		GetClientRect(g_hWnd, &rc);

		int nHorPos, nVerPos;
		
		nHorPos = [](int nPos, int nScale)
		{
			switch (nScale)
			{
			case 2:
				if (nPos > 640 || nPos < 0)
					nPos = 0;
				return nPos * 2;
			case 3:
				if (nPos > 960 || nPos < 0)
					nPos = 0;
				return nPos * 4 / 3;
			case 4:
				if (nPos > 1280 || nPos < 0)
					nPos = 0;
				return nPos;
			}

			return 0; 
		}(ptCursor.x - rcWnd.left - (int)g_nCxFrame, g_nSize);
		nVerPos = [](int nPos, int nScale)
		{
			switch (nScale)
			{
			case 2:
				if (nPos > 480 || nPos < 0)
					nPos = 0;
				return nPos * 2;
			case 3:
				if (nPos > 720 || nPos < 0)
					nPos = 0;
				return nPos * 4 / 3;
			case 4:
				if (nPos > 960 || nPos < 0)
					nPos = 0;
				return nPos;
			}

			return 0;
		}(ptCursor.y - rcWnd.top - (int)g_nCyCaption - (int)g_nCyFrame, g_nSize);

		int nHorLen = 0, nVerLen = 0;

		string szHor = ITOA(nHorPos);
		string szVer = ITOA(nVerPos);

		for (int i = nHorPos; i > 0; nHorLen++)
			i /= 10;
		for (int i = nVerPos; i > 0; nVerLen++)
			i /= 10;

		if (!(g_nStat & 0x00000001))
		{
			g_hr = CreateGraphicsResources(g_hWnd);

			if (FAILED(g_hr))
			{
				PostQuitMessage((int)g_hr);
			}

			g_nStat |= 0x00000001;
		}

		if (!(g_nStat & 0x00000002))
		{
			g_hr = CreateBitmapResources();

			if (FAILED(g_hr))
			{
				PostQuitMessage((int)g_hr);
			}

			g_nStat |= 0x00000002;
			g_bRTUpdate = FALSE;
		}

		if (!(g_nStat & 0x00000004) || !(g_nReserved[0] % 600))
		{
			for (int i = 0; i < XUSER_MAX_COUNT; i++)
			{
				if (!g_bConnected[i])
				{
					g_bConnected[i] = g_Ctrl[i].CheckState();
				}
			}
			g_nStat |= 0x00000004;
		}

		PaintHandler(g_sceneCurr, nHorPos, nVerPos, g_nSize, g_hWnd, rcWnd, pD2DDC, pDXGISC, pBitmap, pFormat, pBrush, pSourceVoice);

		g_dFPS = FPS();
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	case WM_KEYDOWN:
	{
		if (wParam == VK_SPACE)
		{
			pSourceVoice[g_nBGMID]->Stop();

			g_nBGMID += 1;
			if (g_nBGMID > 4)
				g_nBGMID = 0;

			pSourceVoice[g_nBGMID]->Start(0);
		}
	}
	break;
	case WM_QUIT:
	{
		pSourceVoice[g_nBGMID]->Stop();

		for (int i = 0; i < AUDIO_FILE_COUNT; i++)
		{
			pSourceVoice[i]->DestroyVoice();
		}

		for (int i = 0; i < IMAGE_FILE_COUNT; i++)
		{
			SafeRelease(&pBitmapBrush[i]);
			SafeRelease(&pBitmap[i]);
		}

		SafeRelease(&pFormat);
		SafeRelease(&pDWFactory);
		SafeRelease(&pBrush);
		SafeRelease(&pD2DDC);
		SafeRelease(&pD2DFactory);
		SafeRelease(&pBrush);
		SafeRelease(&pEngine);

		delete buffer->pAudioData;

		CoUninitialize();
	}
	break;
	default:
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}

	return 0;
}

// Dialog window procedure function.
INT_PTR CALLBACK DlgProc(
	_In_ HWND			hDlg,											// A handle of the dialog window.
	_In_ UINT			msg,											// The message.
	_In_ WPARAM			wParam,											// Additional parameter.
	_In_ LPARAM			lParam											// Additional parameter.
)
{
	switch (msg)
	{
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDC_SIZE2X)
		{
			g_nSize = 2;
		}
		if (LOWORD(wParam) == IDC_SIZE3X)
		{
			g_nSize = 3;
		}
		if (LOWORD(wParam) == IDC_SIZE4X)
		{
			g_nSize = 4;
		}
		if (LOWORD(wParam) == IDC_START)
		{
			DestroyWindow(g_hDlg);
		}
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	}
	return (INT_PTR)TRUE;
}

#pragma endregion I AM DESCRIBING MY SITUATION!

#pragma region Function code

// Registers the window class in Windows.
inline WORD MyRegisterClass(
	_In_ HINSTANCE		hInstance										// The instance of the program.
)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = g_szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

// Initializes the main instance and window.
inline BOOL InitInstance(
	_In_ HINSTANCE		hInstance,										// The instance of the program.
	_In_ int			nCmdShow										// Integer, describes show status.
)
{
	g_hInstance = hInstance;

	g_nCyCaption = GetSystemMetrics(SM_CYCAPTION);
	g_nCxFrame = GetSystemMetrics(SM_CXSIZEFRAME) + 4;
	g_nCyFrame = GetSystemMetrics(SM_CYSIZEFRAME) + 4;

	g_hWnd = CreateWindowW(g_szWindowClass, g_szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 320 * g_nSize + 2 * g_nCxFrame, 240 * g_nSize + g_nCyCaption + 2 * g_nCyFrame, nullptr, nullptr, hInstance, nullptr);

	if (!g_hWnd)
	{
		return FALSE;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

// Initializes the dialog window.
inline BOOL InitDialog(
	_In_ HINSTANCE		hInstance,										// The instance of the program.
	_In_ int			nCmdShow										// Integer, describes show status.
)
{
	g_hDlg = CreateDialogParamW(
		hInstance,
		MAKEINTRESOURCE(IDD_DIALOG),
		NULL,
		(DLGPROC)DlgProc,
		0
	);
	ShowWindow(g_hDlg, nCmdShow);

	POINT pt = { 0, 0 };
	MSG msgDlg = { g_hDlg, WM_CREATE, (WPARAM)0, (LPARAM)0, 0, pt };

	while (GetMessageW(&msgDlg, NULL, 0, 0))
	{
		TranslateMessage(&msgDlg);
		DispatchMessageW(&msgDlg);
	}

	return TRUE;
}

// Initializes global strings.
inline BOOL InitString(
	_In_ HINSTANCE		hInstance										// The instance of the program.
)
{
	LoadStringW(hInstance, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MPG, g_szWindowClass, MAX_LOADSTRING);

	WCHAR szAudioFileName[AUDIO_FILE_COUNT][9] = {
		L"BGM2.wav",
		L"BGM3.wav",
		L"BGM4.wav",
		L"BGM5.wav",
		L"BGM6.wav",
		L"SE01.wav",
		L"SE02.wav"
	};
	WCHAR szImageFileName[IMAGE_FILE_COUNT][6] = {
		L"1.png",
		L"2.png",
		L"3.png",
		L"4.png",
		L"5.png",
		L"6.jpg",
		L"7.png"
	};

	WCHAR szPath[MAX_PATH] = L"";
	DWORD nPath = 0;

	nPath = GetCurrentDirectoryW(MAX_PATH, szPath);

	//(_tcsrchr(szPath, L'\\'))[1] = 0;

	for (int i = 0; i < AUDIO_FILE_COUNT; i++)
	{
		_tcscpy_s(g_szAudioFilePath[i], szPath);

		g_szAudioFilePath[i][nPath] = '\\';

		for (int j = 0; j < 9; j++)
		{
			g_szAudioFilePath[i][nPath + j + 1] = szAudioFileName[i][j];
		}

		g_szAudioFilePath[i][nPath + 9 + 1] = 52428;
	}

	for (int i = 0; i < IMAGE_FILE_COUNT; i++)
	{
		_tcscpy_s(g_szImageFilePath[i], szPath);

		g_szImageFilePath[i][nPath] = '\\';

		for (int j = 0; j < 6; j++)
		{
			g_szImageFilePath[i][nPath + j + 1] = szImageFileName[i][j];
		}

		g_szImageFilePath[i][nPath + 6 + 1] = 52428;
	}

	return TRUE;
}

// Initializes some global variables.
// Return TRUE for success, FALSE for failure.
constexpr BOOL InitVariable() noexcept
{
	g_hr = S_OK;
	g_nReserved[0] = 0;
	g_nReserved[1] = 1;
	g_nSize = 2;
	g_nStat = 0x00000000;
	g_nBGMID = 0;

	for (int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		g_bConnected[i] = FALSE;
		g_Ctrl[i].InitState(i);
	}

	return TRUE;
}

// Creates graphics resources.
inline HRESULT CreateGraphicsResources(HWND hWnd)
{
	HRESULT hr;

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	hr = D3D11CreateDevice(
		pDXGIAdapter,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		g_nCreationFlags,
		featureLevels,
		9,
		D3D11_SDK_VERSION,
		&pD3DDevice,
		&g_FeatureLevel,
		&pD3DDC
	);

	if (SUCCEEDED(hr))
	{
		hr = pD3DDevice->QueryInterface(
			__uuidof(IDXGIDevice1),
			(void**)&pDXGIDevice
		);

		if (SUCCEEDED(hr))
		{
			hr = D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED,
				__uuidof(ID2D1Factory1),
				reinterpret_cast<void**>(&pD2DFactory)
			);

			if (SUCCEEDED(hr))
			{
				hr = pD2DFactory->CreateDevice(
					pDXGIDevice,
					&pD2DDevice
				);

				if (SUCCEEDED(hr))
				{
					hr = DCompositionCreateDevice(
						pDXGIDevice,
						__uuidof(IDCompositionDevice),
						reinterpret_cast<void**>(&pDCompDevice)
					);

					if (SUCCEEDED(hr))
					{
						hr = pD2DDevice->CreateDeviceContext(
							D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
							&pD2DDC
						);

						if (SUCCEEDED(hr))
						{
							hr = pDXGIDevice->GetAdapter(
								&pDXGIAdapter
							);

							if (SUCCEEDED(hr))
							{
								hr = pDXGIAdapter->GetParent(
									IID_PPV_ARGS(&pDXGIFactory)
								);

								if (SUCCEEDED(hr))
								{
									RECT rc;
									DXGI_SWAP_CHAIN_FULLSCREEN_DESC fscrDesc;
									DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

									GetClientRect(hWnd, &rc);

									fscrDesc.RefreshRate.Numerator = 60;
									fscrDesc.RefreshRate.Denominator = 1;
									fscrDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
									fscrDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
									fscrDesc.Windowed = FALSE;

									swapChainDesc.Width = rc.right - rc.left;
									swapChainDesc.Height = rc.bottom - rc.top;
									swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
									swapChainDesc.Stereo = FALSE;
									swapChainDesc.SampleDesc.Count = 1;
									swapChainDesc.SampleDesc.Quality = 0;
									swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
									swapChainDesc.BufferCount = 2;
									swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
									swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
									swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
									swapChainDesc.Flags = 0;

									hr = pDXGIFactory->CreateSwapChainForHwnd(
										pD3DDevice,
										hWnd,
										&swapChainDesc,
										nullptr,
										nullptr,
										&pDXGISC
									);

									if (SUCCEEDED(hr))
									{
										hr = pDXGIDevice->SetMaximumFrameLatency(1);

										if (SUCCEEDED(hr))
										{
											hr = pDXGISC->GetBuffer(
												0,
												IID_PPV_ARGS(&pDXGISurfaceBackBuffer)
											);

											if (SUCCEEDED(hr))
											{
												D2D1_BITMAP_PROPERTIES1 bitmapProperties = BitmapProperties1(
													D2D1_BITMAP_OPTIONS_CANNOT_DRAW | D2D1_BITMAP_OPTIONS_TARGET,
													PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
												);

												hr = pD2DDC->CreateBitmapFromDxgiSurface(
													pDXGISurfaceBackBuffer,
													&bitmapProperties,
													&pD2DTargetBitmap
												);

												if (SUCCEEDED(hr))
												{
													hr = pD2DDC->CreateSolidColorBrush(
														ColorF(0.4f, 0.6f, 0.0f),
														&pBrush
													);

													if (SUCCEEDED(hr))
													{
														pD2DDC->SetTarget(pD2DTargetBitmap);
														pD2DDC->SetUnitMode(D2D1_UNIT_MODE_PIXELS);

														hr = DWriteCreateFactory(
															DWRITE_FACTORY_TYPE_SHARED,
															__uuidof(IDWriteFactory),
															reinterpret_cast<IUnknown**> (&pDWFactory)
														);

														if (SUCCEEDED(hr))
														{
															hr = pDWFactory->CreateTextFormat(
																L"Comic Sans MS",
																NULL,
																DWRITE_FONT_WEIGHT_NORMAL,
																DWRITE_FONT_STYLE_NORMAL,
																DWRITE_FONT_STRETCH_NORMAL,
																16.0f * (96.0f / 72.0f),
																L"en-US",
																&pFormat
															);

															if (SUCCEEDED(hr))
															{
																hr = CoCreateInstance(
																	CLSID_WICImagingFactory2,
																	NULL,
																	CLSCTX_INPROC_SERVER,
																	IID_PPV_ARGS(&pWICFactory)
																);

																if (SUCCEEDED(hr))
																{
																	hr = pD2DDC->CreateEffect(
																		CLSID_D2D1GaussianBlur,
																		&pD2DEffect
																	);

																	if (SUCCEEDED(hr))
																	{
																		hr = pD2DEffect->SetValue(
																			D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
																			233.33f
																		);

																		if (SUCCEEDED(hr))
																		{
																			hr = pDCompDevice->CreateTargetForHwnd(
																				hWnd,
																				TRUE,
																				&pDCompTarget
																			);

																			if (SUCCEEDED(hr))
																			{
																				hr = pDCompDevice->CreateVisual(
																					&pDCompVisual
																				);

																				if (SUCCEEDED(hr))
																				{
																					// ?
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return hr;
}

// Creates Direct2D bitmaps.
inline HRESULT CreateBitmapResources()
{
	HRESULT hr = S_OK;

	for (int i = 0; i < IMAGE_FILE_COUNT; i++)
	{
		LPCWSTR szTemp = g_szImageFilePath[i];

		hr = LoadBitmapFromFile(
			pD2DDC,
			pWICFactory,
			szTemp,
			g_nSize * 320,
			g_nSize * 240,
			&pBitmap[i],
			&pWICBitmap[i]
		);

		if (SUCCEEDED(hr))
		{
			hr = pD2DDC->CreateBitmapBrush(
				pBitmap[i],
				&pBitmapBrush[i]
			);
		}
	}

	return hr;
}

// Creates XAudio2 resources.
inline HRESULT CreateXA2Resources()
{
	HRESULT hr = S_OK;

	hr = XAudio2Create(&pEngine, 0, XAUDIO2_DEFAULT_PROCESSOR);

	if (SUCCEEDED(hr))
	{
		hr = pEngine->CreateMasteringVoice(&pMasterVoice);
	}

	return hr;
}

// Creates XAudio2 source voices.
inline HRESULT CreateVoiceResources()
{
	HRESULT hr = S_OK;

	for (int i = 0; i < AUDIO_FILE_COUNT; i++)
	{
		LPCWSTR szTemp = g_szAudioFilePath[i];
		
		hr = LoadWaveToBuffer(szTemp, i);

		if (FAILED(hr))
		{
			break;
		}
		else
		{
			hr = pEngine->CreateSourceVoice(&pSourceVoice[i], (WAVEFORMATEX*)&wfx);

			if (FAILED(hr))
			{
				break;
			}
			else
			{
				hr = pSourceVoice[i]->SubmitSourceBuffer(&buffer[i]);
			}
		}
	}

	return hr;
}

// Gives the frames rendered in the last second.
inline double FPS() noexcept
{
	static double FPS = 0.0f;
	static int frameCnt = 0;
	static LINT freq;
	static LINT prevTime, currTime;

	frameCnt++;

	if (!(g_nStat & 0x00000008))
	{
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&prevTime);
		QueryPerformanceCounter(&currTime);
		g_nStat |= 0x0008;
	}

	QueryPerformanceCounter(&currTime);

	if ((currTime.QuadPart - prevTime.QuadPart) >= freq.QuadPart)
	{
		double time = ((double)(currTime.QuadPart - prevTime.QuadPart)) / (double)(freq.QuadPart);

		g_dFPS = ((double)frameCnt) / time;

		frameCnt = 0;
		QueryPerformanceCounter(&prevTime);
	}

	return g_dFPS;
}

#pragma endregion Maybe Cell knows what is going on here.


