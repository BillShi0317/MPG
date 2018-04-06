#pragma once

#include "stdafx.h"
#include "transform.h"

#define AUDIO_FILE_COUNT 7

#ifdef _XBOX					// Big-endian.
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT  'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX					// Little-endian.
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT  ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

inline HRESULT FindChunk(HANDLE, DWORD, DWORD &, DWORD &);
inline HRESULT ReadChunkData(HANDLE, void*, DWORD, DWORD);
inline HRESULT LoadWaveToBuffer(LPCWSTR, UINT);

WAVEFORMATEXTENSIBLE wfx[AUDIO_FILE_COUNT] = { 0 };
XAUDIO2_BUFFER buffer[AUDIO_FILE_COUNT] = { 0 };



inline HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK)
	{
		DWORD dwRead;
		if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;

		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
				return HRESULT_FROM_WIN32(GetLastError());
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;

	}

	return S_OK;
}

inline HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;

	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwRead;

	if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());

	return hr;
}

inline HRESULT LoadWaveToBuffer(LPCWSTR strPath, UINT i)
{
	wfx[i] = { 0 };
	buffer[i] = { 0 };

	HRESULT hr = S_OK;

	HANDLE hAudioFile = NULL;

	hAudioFile = CreateFile(
		strPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		0,
		NULL
	);

	if (INVALID_HANDLE_VALUE == hAudioFile)
		return HRESULT_FROM_WIN32(GetLastError());

	if (INVALID_SET_FILE_POINTER == SetFilePointer(hAudioFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	DWORD filetype;

	hr = FindChunk(hAudioFile, fourccRIFF, dwChunkSize, dwChunkPosition);
	if (SUCCEEDED(hr))
	{
		hr = ReadChunkData(hAudioFile, &filetype, sizeof(DWORD), dwChunkPosition);

		if (SUCCEEDED(hr))
		{
			if (filetype != fourccWAVE)
				return S_FALSE;

			hr = FindChunk(hAudioFile, fourccFMT, dwChunkSize, dwChunkPosition);

			if (SUCCEEDED(hr))
			{
				hr = ReadChunkData(hAudioFile, &wfx[i], dwChunkSize, dwChunkPosition);

				if (SUCCEEDED(hr))
				{
					hr = FindChunk(hAudioFile, fourccDATA, dwChunkSize, dwChunkPosition);

					if (SUCCEEDED(hr))
					{
						try
						{
							BYTE* pDataBuffer = new BYTE[dwChunkSize];

							hr = ReadChunkData(hAudioFile, pDataBuffer, dwChunkSize, dwChunkPosition);

							if (SUCCEEDED(hr))
							{
								if (i < 5)
								{
									buffer[i].AudioBytes = dwChunkSize;
									buffer[i].Flags = XAUDIO2_END_OF_STREAM;
									buffer[i].LoopBegin = 0;
									buffer[i].LoopCount = XAUDIO2_LOOP_INFINITE;
									buffer[i].LoopLength = 0;
									buffer[i].pAudioData = pDataBuffer;
								}
								else
								{
									buffer[i].AudioBytes = dwChunkSize;
									buffer[i].Flags = XAUDIO2_END_OF_STREAM;
									buffer[i].LoopBegin = 0;
									buffer[i].LoopCount = 0;
									buffer[i].LoopLength = 0;
									buffer[i].pAudioData = pDataBuffer;
								}
							}
						}
						catch (bad_alloc & ba)
						{
							MessageBoxW(NULL, ATOU(ba.what()), L"FUCK", MB_OK);
						}
					}
				}
			}
		}
	}

	return hr;
}


