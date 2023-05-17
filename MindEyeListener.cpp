#include "pch.h"
#include "MindEyeListener.h"

BOOL ListenerInfoDto::operator==(const ListenerInfoDto &listenerDto) const
{
    return UID == listenerDto.UID;
}

BOOL ListenerInfoDto::operator<(const ListenerInfoDto &listenerDto) const
{
    return UID < listenerDto.UID;
}

BOOL ListenerInfoDto::operator>(const ListenerInfoDto &listenerDto) const
{
    return UID > listenerDto.UID;
}

MindEyeListener::MindEyeListener(ListenerInfoDto infoIn)
{
    UID = infoIn.UID;
    pSubmixVoice = NULL;
    listener = {};
    listener.Position = {infoIn.PX, infoIn.PY, infoIn.PZ};
    listener.Velocity = {infoIn.VX, infoIn.VY, infoIn.VZ};
}

MindEyeListener::~MindEyeListener()
{
}

HRESULT MindEyeListener::Init(IXAudio2 *pXAudio2, WAVEFORMATEXTENSIBLE WFX)
{
    HRESULT hr = pXAudio2->CreateSubmixVoice(
        &pSubmixVoice, WFX.Format.nChannels, WFX.Format.nSamplesPerSec);
    return hr;
}
