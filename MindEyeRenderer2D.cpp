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
    LARGE_INTEGER performanceCountStart = {};
    LARGE_INTEGER performanceCountStop = {};
    SpaceInfo spaceInfoEmitter = params.spaceInfoEmitter;
    SpaceInfo spaceInfoListener = params.spaceInfoListener;
    fftw_plan plan;
    // 时域大小
    DWORD dwSizeTD = cFrames;
    // 频域大小
    DWORD dwSizeFD = dwSizeTD / 2 + 1;
    // 时域数据
    double *pDataTD = new double[dwSizeTD];
    for (DWORD i = 0; i < dwSizeTD; i++)
    {
        pDataTD[i] = (double)pData[i];
    }
    // 频域数据
    fftw_complex *pDataFD = new fftw_complex[dwSizeFD];
    // 正向变换
    plan = fftw_plan_dft_r2c_1d(dwSizeTD, pDataTD, pDataFD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    // 逆向变换
    plan = fftw_plan_dft_c2r_1d(dwSizeTD, pDataFD, pDataTD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    for (DWORD i = 0; i < dwSizeTD; i++)
    {
        pData[i] = (FLOAT32)pDataTD[i] / dwSizeTD;
    }

    delete[] pDataTD, pDataFD;
}
