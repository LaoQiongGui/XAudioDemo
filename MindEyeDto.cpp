#include "pch.h"
#include "MindEyeDto.h"

// MindEyeEmitter
MindEyeEmitter::~MindEyeEmitter()
{
    if (pAudio2SourceVoiceL != NULL)
    {
        pAudio2SourceVoiceL->Stop();
        pAudio2SourceVoiceL->DestroyVoice();
    }
    if (pAudio2SourceVoiceR != NULL)
    {
        pAudio2SourceVoiceR->Stop();
        pAudio2SourceVoiceR->DestroyVoice();
    }
}