#pragma once
#include "Common.h"
#include "MindEyeRendererBase.h"

struct MindEyeRenderer2DParams
{
	SpaceInfo spaceInfoEmitter;
	SpaceInfo spaceInfoListener;
};

class __declspec(uuid("{5EB8D611-FF96-429d-8365-2DDF89A7C1CD}"))
	MindEyeRenderer2D : public MindEyeRendererBase<MindEyeRenderer2D, MindEyeRenderer2DParams>
{
public:
	MindEyeRenderer2D();
	~MindEyeRenderer2D();

	void DoProcess(const MindEyeRenderer2DParams &, _Inout_updates_all_(cFrames *cChannels) FLOAT32 *__restrict pData, UINT32 cFrames, UINT32 cChannels) override;
};
