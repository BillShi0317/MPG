#pragma once

#include "stdafx.h"
#include "transform.h"

#define IMAGE_FILE_COUNT 7



inline HRESULT CreateGraphicsResources9(HWND);
inline HRESULT CreateGraphicsResources10(HWND);
inline HRESULT CreateGraphicsResources11(HWND);
inline HRESULT LoadBitmapFromFile(ID2D1RenderTarget*, IWICImagingFactory2*, LPCWSTR, UINT, UINT, ID2D1Bitmap**, IWICBitmap**);
inline HRESULT ScaleBitmap(ID2D1Factory*, IWICImagingFactory*, IWICBitmap*, double, double, IWICBitmap**);
inline HRESULT RotateBitmap(ID2D1Factory*, IWICImagingFactory*, IWICBitmap*, double, IWICBitmap**);





inline HRESULT CreateGraphicsResources9(HWND)
{

}

// Load bitmap from the directory given.
inline HRESULT LoadBitmapFromFile(
	_In_ ID2D1RenderTarget*		pRT,										// The pointer to render target interface.
	_In_ IWICImagingFactory2*	pIWICFactory,								// The pointer to Windows Imaging Component factory interface.
	_In_ LPCWSTR				szPath,										// The path of the image file.
	_In_ UINT					uDestWidth,									// The destination width.
	_In_ UINT					uDestHeight,								// The destination height.
	_Out_ ID2D1Bitmap**			ppBitmap,									// The pointer to a pointer to the bitmap interface.
	_Out_ IWICBitmap**			ppBmp
)
{
	HRESULT hr = S_OK;

	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	hr = pIWICFactory->CreateDecoderFromFilename(
		szPath,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pSource);

		if (SUCCEEDED(hr))
		{
			hr = pIWICFactory->CreateFormatConverter(&pConverter);

			if (uDestWidth != 0 || uDestHeight != 0)
			{
				UINT uOrigWidth, uOrigHeight;

				hr = pSource->GetSize(&uOrigWidth, &uOrigHeight);

				if (SUCCEEDED(hr))
				{
					if (uDestWidth == 0)
					{
						FLOAT scaler = static_cast<FLOAT>(uDestHeight) / static_cast<FLOAT>(uOrigHeight);
						uDestWidth = static_cast<UINT>(scaler * static_cast<FLOAT>(uOrigWidth));
					}
					else if (uDestHeight == 0)
					{
						FLOAT scaler = static_cast<FLOAT>(uDestWidth) / static_cast<FLOAT>(uOrigWidth);
						uDestHeight = static_cast<UINT>(scaler * static_cast<FLOAT>(uOrigHeight));
					}

					hr = pIWICFactory->CreateBitmapScaler(&pScaler);

					if (SUCCEEDED(hr))
					{
						hr = pScaler->Initialize(
							pSource,
							uDestWidth,
							uDestHeight,
							WICBitmapInterpolationModeCubic
						);

						if (SUCCEEDED(hr))
						{
							hr = pConverter->Initialize(
								pScaler,
								GUID_WICPixelFormat32bppPBGRA,
								WICBitmapDitherTypeNone,
								NULL,
								0.f,
								WICBitmapPaletteTypeMedianCut
							);

							if (SUCCEEDED(hr))
							{
								hr = pRT->CreateBitmapFromWicBitmap(
									pConverter,
									NULL,
									ppBitmap
								);

								if (SUCCEEDED(hr))
								{
									UINT uW;
									UINT uH;

									pConverter->GetSize(&uW, &uH);

									hr = pIWICFactory->CreateBitmapFromSourceRect(
										pConverter,
										0,
										0,
										uW,
										uH,
										ppBmp
									);
								}
							}
						}
					}
				}
			}
		}
	}

	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pStream);
	SafeRelease(&pConverter);
	SafeRelease(&pScaler);

	return hr;
}

// Transforms a bitmap with the given transformation.
inline HRESULT ScaleBitmap(
	_In_ ID2D1Factory*			pD2DFactory,
	_In_ IWICImagingFactory*	pWICFactory,
	_In_ IWICBitmap*			pBmp,
	_In_ double					dXScale,
	_In_ double					dYScale,
	_Out_ IWICBitmap**			ppWICBmp
)
{
	HRESULT hr = S_OK;
	UINT uOrigWidth;
	UINT uOrigHeight;

	pBmp->GetSize(&uOrigWidth, &uOrigHeight);

	float fDestWidth = (double)dXScale * (double)uOrigWidth;
	float fDestHeight = (double)dYScale * (double)uOrigHeight;

	ID2D1RenderTarget* pRT = NULL;
	ID2D1Bitmap* pD2DBmp = NULL;

	SafeRelease(ppWICBmp);

	hr = pWICFactory->CreateBitmap(
		fDestWidth + 1,
		fDestHeight + 1,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapCacheOnDemand,
		ppWICBmp
	);

	if (SUCCEEDED(hr))
	{
		hr = pD2DFactory->CreateWicBitmapRenderTarget(
			*ppWICBmp,
			RenderTargetProperties(),
			&pRT
		);

		if (SUCCEEDED(hr))
		{
			hr = pRT->CreateBitmapFromWicBitmap(
				pBmp,
				&pD2DBmp
			);

			if (SUCCEEDED(hr))
			{
				pRT->BeginDraw();

				pRT->Clear(ColorF(1.0f, 1.0f, 1.0f, 0.0f));

				pRT->DrawBitmap(
					pD2DBmp,
					RectF(0, 0, fDestWidth, fDestHeight),
					1.0f,
					D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
					RectF(0, 0, uOrigWidth, uOrigHeight)
				);

				hr = pRT->EndDraw();

				if (SUCCEEDED(hr))
				{
					SafeRelease(&pRT);
					SafeRelease(&pD2DBmp);
				}
			}
		}
	}

	return hr;
}

// Hahaha I don't want to give an explanation just guess hahahahahahah
inline HRESULT RotateBitmap(
	_In_ ID2D1Factory*			pD2DFactory,
	_In_ IWICImagingFactory*	pWICFactory,
	_In_ IWICBitmap*			pBmp,
	_In_ double					dAngle,
	_Out_ IWICBitmap**			ppWICBmp
)
{
	HRESULT hr = S_OK;
	UINT uOrigWidth;
	UINT uOrigHeight;
	double rad = dAngle * PI / 180.0f;

	pBmp->GetSize(&uOrigWidth, &uOrigHeight);

	float fDestWidth = max(abs((float)uOrigWidth * cos(rad) + (float)uOrigHeight * sin(rad)), abs((float)uOrigWidth * cos(rad) - (float)uOrigHeight * sin(rad)));
	float fDestHeight = max(abs((float)uOrigWidth * sin(rad) + (float)uOrigHeight * cos(rad)), abs((float)uOrigWidth * sin(rad) - (float)uOrigHeight * cos(rad)));

	ID2D1RenderTarget* pRT = NULL;
	ID2D1Bitmap* pD2DBmp = NULL;

	hr = pWICFactory->CreateBitmap(
		fDestWidth + 1,
		fDestHeight + 1,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapCacheOnDemand,
		ppWICBmp
	);

	if (SUCCEEDED(hr))
	{
		hr = pD2DFactory->CreateWicBitmapRenderTarget(
			*ppWICBmp,
			RenderTargetProperties(),
			&pRT
		);

		if (SUCCEEDED(hr))
		{
			hr = pRT->CreateBitmapFromWicBitmap(
				pBmp,
				&pD2DBmp
			);

			if (SUCCEEDED(hr))
			{
				pRT->BeginDraw();

				pRT->Clear(ColorF(1.0f, 1.0f, 1.0f, 0.0f));

				D2D1_POINT_2F offset = Point2F((float)(fDestWidth - uOrigWidth) / 2.0f, (float)(fDestHeight - uOrigHeight) / 2.0f);
				D2D1_POINT_2F center = Point2F((float)(fDestWidth / 2.0f), (float)(fDestHeight / 2.0f));
				Matrix3x2F mat = Matrix3x2F::Rotation(dAngle, center);

				pRT->SetTransform(mat);

				pRT->DrawBitmap(
					pD2DBmp,
					RectF(offset.x, offset.y, uOrigWidth + offset.x, uOrigHeight + offset.y)
				);

				hr = pRT->EndDraw();

				if (SUCCEEDED(hr))
				{
					SafeRelease(&pRT);
					SafeRelease(&pD2DBmp);
				}
			}
		}
	}

	return hr;
}


