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
 * \param cFrames：
 * \param cChannels：通道数
 */
void MindEyeRenderer2D::DoProcess(const MindEyeRenderer2DParams &params, FLOAT32 *pData, UINT32 cFrames, UINT32 cChannels)
{
    SpaceInfo spaceInfoEmitter = params.spaceInfoEmitter;
    SpaceInfo spaceInfoListener = params.spaceInfoListener;
    for (UINT32 i = 0; i < cFrames * cChannels; ++i)
    {
        // TODO：音频响度处理
        // pData[i] = 0;
    }
}
