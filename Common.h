#define DEBUG_MODE
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT // 开启spdlog的wchar支持
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE // 开启spdlog的文件名+行号打印

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
#include <functional>
#include <set>
#include <map>
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
#include <roapi.h>
#include <sapi.h>
#include <sphelper.h>
#include "resource.h"

#ifdef DEBUG_MODE
#define MYDEBUG(x) x
#else
#define MYDEBUG(x)
#endif

// 全局变量
extern std::shared_ptr<spdlog::logger> logger;
extern CHAR *logStr;

// 查找与读取文件区块
HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD &dwChunkSize, DWORD &dwChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void *buffer, DWORD buffersize, DWORD bufferoffset);

// 音频文件分频
HRESULT DivideFrequency(HANDLE hFileIn, HANDLE *hFileOutList, DWORD *dwFrequencyList, DWORD dwSize);

// 设置音频格式为FLOAT32
void SetWFXF32(WAVEFORMATEX *pWFX);

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

    WavFileStruct::WavFileStruct(WAVEFORMATEX WFX = WAVEFORMATEX(), DWORD dwBufferSize = 0);
    void FillWAVEFORMATEX(WAVEFORMATEX WFX);
    void FillBufferSize(DWORD dwBufferSize);
};

class MindEyeWaveFile
{
public:
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

    HRESULT Read(WCHAR *wLoadPath);
    HRESULT Save(WCHAR *wSavePath);

    template <typename T>
    VOID TransForm();
};

template <typename T>
VOID inline MindEyeWaveFile::TransForm()
{
}
