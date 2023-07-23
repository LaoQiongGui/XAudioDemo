#ifdef _XBOX // Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX // Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <set>
#include <deque>
#include <algorithm>
#include <exception>
#include <Windows.h>
#include <WinUser.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <process.h>
#include <WinUser.h>
#include <d2d1.h>
#include <xaudio2.h>
#include <x3daudio.h>
#include <xapobase.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <profileapi.h>
#include <fftw3.h>
#include "resource.h"

// DWORD转WCHAR*
WCHAR *DWORD2WCHAR(DWORD dwInput);
WCHAR *DWORD2WCHAR(DWORD dwInput, DWORD dwMode);
WCHAR *FLOAT2WCHAR(FLOAT fInput);

HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD &dwChunkSize, DWORD &dwChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void *buffer, DWORD buffersize, DWORD bufferoffset);

HRESULT DivideFrequency(HANDLE hFileIn, HANDLE *hFileOutList, DWORD *dwFrequencyList, DWORD dwSize);

struct WavFileStruct
{
    // RIFF区块
    DWORD dwChunkID;
    DWORD dwChunkSize;
    DWORD dwFormat;

    // fmt区块
    DWORD dwSubchunk1ID;
    DWORD dwSubchunk1Size;
    WORD wAudioFormat;
    WORD wNumChannels;
    DWORD dwSampleRate;
    DWORD dwByteRate;
    WORD wBlockAlign;
    WORD wBitsPerSample;

    // data区块
    DWORD dwSubchunk2ID;
    DWORD dwSubchunk2Size;

    WavFileStruct();
    WavFileStruct(WAVEFORMATEX WFX, DWORD dwBufferSize);
    void FillWAVEFORMATEX(WAVEFORMATEX WFX);
    void FillBufferSize(DWORD dwBufferSize);
};
