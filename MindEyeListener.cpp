#include "pch.h"
#include "MindEyeListener.h"

BOOL ListenerInfoDto::operator==(const ListenerInfoDto &listenerDto) const
{
    return ID == listenerDto.ID;
}

BOOL ListenerInfoDto::operator<(const ListenerInfoDto &listenerDto) const
{
    return ID < listenerDto.ID;
}

BOOL ListenerInfoDto::operator>(const ListenerInfoDto &listenerDto) const
{
    return ID > listenerDto.ID;
}

MindEyeListener::MindEyeListener(ListenerInfoDto infoIn)
{
    ID = infoIn.ID;
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
    return ID == listenerDto.ID;
}

BOOL MindEyeListener::operator<(const ListenerInfoDto &listenerDto) const
{
    return ID < listenerDto.ID;
}

BOOL MindEyeListener::operator>(const ListenerInfoDto &listenerDto) const
{
    return ID > listenerDto.ID;
}

BOOL MindEyeListener::operator==(const MindEyeListener &mindEyeListener) const
{
    return ID == mindEyeListener.ID;
}

BOOL MindEyeListener::operator<(const MindEyeListener &mindEyeListener) const
{
    return ID < mindEyeListener.ID;
}

BOOL MindEyeListener::operator>(const MindEyeListener &mindEyeListener) const
{
    return ID > mindEyeListener.ID;
}
