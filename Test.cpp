#include "pch.h"
#include "Test.h"

WCHAR wWavFilePath_1[] = L"D:/Workspace/LaoQiongGui/XAudioDemo/resources/sound_effect_sample_31.wav";
WCHAR wWavFilePath_2[] = L"D:/Workspace/LaoQiongGui/XAudioDemo/resources/sound_effect_sample_32.wav";
MindEyeEmitterInfo mindEyeEmitterInfo_1;
MindEyeEmitterInfo mindEyeEmitterInfo_2;
MindEyeListenerInfo mindEyeListenerInfo_1;
MindEyeListenerInfo mindEyeListenerInfo_2;

HRESULT testInit()
{
    DWORD dwChunkSize = 0;
    DWORD dwChunkPosition = 0;
    HRESULT hr = S_OK;
    HANDLE hFile = NULL;
    BYTE *pDataBuffer = NULL;

    // 设置音频1
    mindEyeEmitterInfo_1.spaceInfo = SpaceInfo();

    // 打开音频文件
    hFile = ::CreateFile(
        wWavFilePath_1, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, 0, NULL);
    ASSERT(hFile != NULL);

    // 读取音频数据
    FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
    pDataBuffer = new BYTE[dwChunkSize];
    ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

    // 关闭音频文件（读取）
    ::CloseHandle(hFile);
    mindEyeEmitterInfo_1.bufferSize = dwChunkSize;
    mindEyeEmitterInfo_1.pBufferData = pDataBuffer;

    // 设置音频2
    mindEyeEmitterInfo_2.spaceInfo = SpaceInfo();

    // 打开音频文件
    hFile = ::CreateFile(
        wWavFilePath_2, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, 0, NULL);
    ASSERT(hFile != NULL);

    // 读取音频数据
    FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
    pDataBuffer = new BYTE[dwChunkSize];
    ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

    // 关闭音频文件（读取）
    ::CloseHandle(hFile);
    mindEyeEmitterInfo_2.bufferSize = dwChunkSize;
    mindEyeEmitterInfo_2.pBufferData = pDataBuffer;

    return hr;
}

std::map<DWORD64, MindEyeEmitterInfo *> getEmitterInfoForTest()
{
    std::map<DWORD64, MindEyeEmitterInfo*> emitterInfoMap;
    if (
        (!mindEyeEmitterInfo_1.pBufferData ||
         !mindEyeEmitterInfo_2.pBufferData) &&
        testInit() != S_OK)
    {
        // 初始化失败返回空set
        return emitterInfoMap;
    }
    emitterInfoMap[1] = &mindEyeEmitterInfo_1;
    emitterInfoMap[2] = &mindEyeEmitterInfo_2;
    return emitterInfoMap;
}

std::map<DWORD64, MindEyeListenerInfo *> getListenerInfoForTest()
{
    std::map<DWORD64, MindEyeListenerInfo *> listenerInfoMap;
    mindEyeListenerInfo_1.spaceInfo = SpaceInfo();
    mindEyeListenerInfo_2.spaceInfo = SpaceInfo();
    listenerInfoMap[1] = &mindEyeListenerInfo_1;
    listenerInfoMap[2] = &mindEyeListenerInfo_2;
    return listenerInfoMap;
}
