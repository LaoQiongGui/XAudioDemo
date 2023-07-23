#include "pch.h"
#include "Common.h"

// DWORD转WCHAR*
WCHAR *DWORD2WCHAR(DWORD dwInput)
{
    WCHAR *buffer = new WCHAR[MAX_PATH];
    swprintf_s(buffer, MAX_PATH, L"%u", dwInput);
    return buffer;
}

WCHAR *DWORD2WCHAR(DWORD dwInput, DWORD dwMode)
{
    WCHAR *buffer = new WCHAR[MAX_PATH];
    switch (dwMode)
    {
    case 0:
        swprintf_s(buffer, MAX_PATH, L"%d", dwInput);
        break;
    case 1:
        swprintf_s(buffer, MAX_PATH, L"%x", dwInput);
        break;
    default:
        swprintf_s(buffer, MAX_PATH, L"%u", dwInput);
        break;
    }
    return buffer;
}

WCHAR *FLOAT2WCHAR(FLOAT fInput)
{
    WCHAR *buffer = new WCHAR[MAX_PATH];
    swprintf_s(buffer, MAX_PATH, L"%f", fInput);
    return buffer;
}

HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD &dwChunkSize, DWORD &dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkType = 0;
    DWORD dwChunkDataSize = 0;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType = 0;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
            break;

        default:
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
            {
                return HRESULT_FROM_WIN32(GetLastError());
            }
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize)
        {
            return S_FALSE;
        }
    }

    return S_OK;
}

HRESULT ReadChunkData(HANDLE hFile, void *buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }
    return hr;
}

/**
 * 对输入音频分频.
 *
 * \param hFileIn：输入音频文件句柄
 * \param hFileOutList：输出音频文件句柄
 * \param dwFrequencyList：各输出音频中心频率
 * \param dwSize：输出音频数量
 * \return
 */
HRESULT DivideFrequency(HANDLE hFileIn, HANDLE *hFileOutList, DWORD *dwFrequencyList, DWORD dwSize)
{
    BYTE *pHeader, *pBufferL, *pBufferS;
    DWORD dwZero = 0;

    DWORD dwChunkSize, dwChunkPosition;
    // 起始采样点，中心采样点，截止采样点
    DWORD dwStart, dwCenter, dwEnd;
    fftw_plan plan;
    DOUBLE *pTDBufferL, *pTDBufferS;
    fftw_complex *pFDBufferL, *pFDBufferS;

    FindChunk(hFileIn, fourccDATA, dwChunkSize, dwChunkPosition);
    pHeader = new BYTE[dwChunkPosition];
    pBufferL = new BYTE[dwChunkSize];
    pBufferS = new BYTE[dwChunkSize];
    // 读取文件头
    ReadChunkData(hFileIn, pHeader, dwChunkPosition, 0);
    // 读取音频数据
    ReadChunkData(hFileIn, pBufferL, dwChunkSize, dwChunkPosition);
    ::CloseHandle(hFileIn);

    DWORD dwSizeTD = dwChunkSize / 2;
    DWORD dwSizeFD = dwSizeTD / 2 + 1;
    pTDBufferL = new DOUBLE[dwSizeTD];
    pTDBufferS = new DOUBLE[dwSizeTD];
    pFDBufferL = new fftw_complex[dwSizeFD];
    pFDBufferS = new fftw_complex[dwSizeFD];

    // 正向变换
    plan = fftw_plan_dft_r2c_1d(dwSizeTD, pTDBufferL, pFDBufferL, FFTW_ESTIMATE);
    for (DWORD i = 0; i < dwSizeTD; i++)
    {
        INT16 iBufferTmp = (pBufferL[2 * i + 1] << 8) | pBufferL[2 * i];
        pTDBufferL[i] = iBufferTmp;
    }
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    // 逆向变换
    plan = fftw_plan_dft_c2r_1d(dwSizeTD, pFDBufferS, pTDBufferS, FFTW_ESTIMATE);
    for (DWORD i = 0; i < dwSize; i++)
    {
        memcpy_s(pFDBufferS, dwSizeFD * sizeof(fftw_complex),
                 pFDBufferL, dwSizeFD * sizeof(fftw_complex));
        // 设置起始采样点，中心采样点，截止采样点
        if (i == 0)
        {
            dwStart = 0;
        }
        else
        {
            dwStart = (DWORD)(dwSizeTD * dwFrequencyList[i - 1] / 44100);
        }
        dwCenter = (DWORD)(dwSizeTD * dwFrequencyList[i] / 44100);
        if (i == dwSize - 1)
        {
            dwEnd = dwSizeFD;
        }
        else
        {
            dwEnd = (DWORD)(dwSizeTD * dwFrequencyList[i + 1] / 44100);
        }

        // 滤波
        for (DWORD j = 0; j < dwSizeFD; j++)
        {
            // 低于起始频率或高于截止频率设为0
            DOUBLE dGain;
            if (j < dwStart || j >= dwEnd)
            {
                dGain = 0;
            }
            // 高于起始频率且低于中心频率
            else if (j < dwCenter && i != 0)
            {
                dGain = log((DOUBLE)j / dwStart) / log((DOUBLE)dwCenter / dwStart);
            }
            // 高于中心频率且低于截止频率
            else if (j >= dwCenter && i != dwSize - 1)
            {
                dGain = 1 - log((DOUBLE)j / dwCenter) / log((DOUBLE)dwEnd / dwCenter);
            }
            // 低频部分与高频部分
            else
            {
                dGain = 1;
            }
            pFDBufferS[j][0] *= dGain;
            pFDBufferS[j][1] *= dGain;
        }

        // 执行逆向变换
        fftw_execute(plan);

        for (DWORD j = 0; j < dwSizeTD; j++)
        {
            INT16 wBufferTmp = (INT16)(pTDBufferS[j] / dwSizeTD);
            pBufferS[2 * j] = LOBYTE(wBufferTmp);
            pBufferS[2 * j + 1] = HIBYTE(wBufferTmp);
        }

        // 写入文件头
        ::WriteFile(hFileOutList[i], pHeader, dwChunkPosition, &dwZero, NULL);
        // 写入音频数据
        ::WriteFile(hFileOutList[i], pBufferS, dwSizeTD * 2, &dwZero, NULL);
        ::CloseHandle(hFileOutList[i]);
    }
    fftw_destroy_plan(plan);

    delete[] pHeader, pBufferL, pBufferS;
    delete[] pTDBufferL, pTDBufferS, pFDBufferL, pFDBufferS;
    return S_OK;
}
