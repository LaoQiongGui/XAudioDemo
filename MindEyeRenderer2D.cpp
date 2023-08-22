#include "pch.h"
#include "MindEyeRenderer2D.h"

MindEyeRenderer2D::MindEyeRenderer2D()
{
}

MindEyeRenderer2D::~MindEyeRenderer2D()
{
}

/**
 * 2D音效渲染函数.
 *
 * \param params：MindEyeRenderer2D参数
 * \param pData：音频数据
 * \param cFrames：声音帧数
 * \param cChannels：通道数
 */
void MindEyeRenderer2D::DoProcess(const MindEyeRenderer2DParams &params, FLOAT32 *pData, UINT32 cFrames, UINT32 cChannels)
{
    SpaceInfo spaceInfoEmitter = params.spaceInfoEmitter;
    SpaceInfo spaceInfoListener = params.spaceInfoListener;
    // 时域大小
    DWORD dwSizeTD = cFrames;
    // 频域大小
    DWORD dwSizeFD = dwSizeTD / 2 + 1;
    for (DWORD i = 0; i < dwSizeTD; i++)
    {
        params.pDataTD[i] = (double)pData[i];
    }
    // 正向变换
    fftw_execute(*params.pFftPlan);

    // 逆向变换
    fftw_execute(*params.pIfftPlan);

    for (DWORD i = 0; i < dwSizeTD; i++)
    {
        pData[i] = (FLOAT32)params.pDataTD[i] / dwSizeTD;
    }
    return;
}
