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
    listener.Position = X3DAUDIO_VECTOR();
    listener.Velocity = X3DAUDIO_VECTOR();
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

BOOL MindEyeListener::operator==(const ListenerInfoDto &listenerDto) const
{
    return UID == listenerDto.UID;
}

BOOL MindEyeListener::operator<(const ListenerInfoDto &listenerDto) const
{
    return UID < listenerDto.UID;
}

BOOL MindEyeListener::operator>(const ListenerInfoDto &listenerDto) const
{
    return UID > listenerDto.UID;
}

BOOL MindEyeListener::operator==(const MindEyeListener &mindEyeListener) const
{
    return UID == mindEyeListener.UID;
}

BOOL MindEyeListener::operator<(const MindEyeListener &mindEyeListener) const
{
    return UID < mindEyeListener.UID;
}

BOOL MindEyeListener::operator>(const MindEyeListener &mindEyeListener) const
{
    return UID > mindEyeListener.UID;
}
