#include "pch.h"
#include "MindEyeDto.h"

// MindEyeEmitterInfo
BOOL MindEyeEmitterInfo::operator==(const DWORD64 &ID) const
{
    return MindEyeEmitterInfo::ID == ID;
}

BOOL MindEyeEmitterInfo::operator<(const DWORD64 &ID) const
{
    return MindEyeEmitterInfo::ID < ID;
}

BOOL MindEyeEmitterInfo::operator>(const DWORD64 &ID) const
{
    return MindEyeEmitterInfo::ID > ID;
}

BOOL MindEyeEmitterInfo::operator==(const MindEyeEmitterInfo &info) const
{
    return ID == info.ID;
}

BOOL MindEyeEmitterInfo::operator<(const MindEyeEmitterInfo &info) const
{
    return ID < info.ID;
}

BOOL MindEyeEmitterInfo::operator>(const MindEyeEmitterInfo &info) const
{
    return ID > info.ID;
}

// MindEyeEmitter
MindEyeEmitter::~MindEyeEmitter()
{
    if (pAudio2SourceVoiceL != NULL) {
        pAudio2SourceVoiceL->Stop();
        pAudio2SourceVoiceL->DestroyVoice();
    }
    if (pAudio2SourceVoiceR != NULL) {
        pAudio2SourceVoiceR->Stop();
        pAudio2SourceVoiceR->DestroyVoice();
    }
}

BOOL MindEyeEmitter::operator==(const DWORD64 &ID) const
{
    return MindEyeEmitter::ID == ID;
}

BOOL MindEyeEmitter::operator<(const DWORD64 &ID) const
{
    return MindEyeEmitter::ID < ID;
}

BOOL MindEyeEmitter::operator>(const DWORD64 &ID) const
{
    return MindEyeEmitter::ID > ID;
}

BOOL MindEyeEmitter::operator==(const MindEyeEmitterInfo &info) const
{
    return ID == info.ID;
}

BOOL MindEyeEmitter::operator<(const MindEyeEmitterInfo &info) const
{
    return ID < info.ID;
}

BOOL MindEyeEmitter::operator>(const MindEyeEmitterInfo &info) const
{
    return ID > info.ID;
}

BOOL MindEyeEmitter::operator==(const MindEyeEmitter &emitter) const
{
    return ID == emitter.ID;
}

BOOL MindEyeEmitter::operator<(const MindEyeEmitter &emitter) const
{
    return ID < emitter.ID;
}

BOOL MindEyeEmitter::operator>(const MindEyeEmitter &emitter) const
{
    return ID > emitter.ID;
}

// MindEyeListenerInfo
BOOL MindEyeListenerInfo::operator==(const DWORD64 &ID) const
{
    return MindEyeListenerInfo::ID == ID;
}

BOOL MindEyeListenerInfo::operator<(const DWORD64 &ID) const
{
    return MindEyeListenerInfo::ID < ID;
}

BOOL MindEyeListenerInfo::operator>(const DWORD64 &ID) const
{
    return MindEyeListenerInfo::ID > ID;
}

BOOL MindEyeListenerInfo::operator==(const MindEyeListenerInfo &info) const
{
    return ID == info.ID;
}

BOOL MindEyeListenerInfo::operator<(const MindEyeListenerInfo &info) const
{
    return ID < info.ID;
}

BOOL MindEyeListenerInfo::operator>(const MindEyeListenerInfo &info) const
{
    return ID > info.ID;
}

// MindEyeListener
BOOL MindEyeListener::operator==(const DWORD64 &ID) const
{
    return MindEyeListener::ID == ID;
}

BOOL MindEyeListener::operator<(const DWORD64 &ID) const
{
    return MindEyeListener::ID < ID;
}

BOOL MindEyeListener::operator>(const DWORD64 &ID) const
{
    return MindEyeListener::ID > ID;
}

BOOL MindEyeListener::operator==(const MindEyeListenerInfo &info) const
{
    return ID == info.ID;
}

BOOL MindEyeListener::operator<(const MindEyeListenerInfo &info) const
{
    return ID < info.ID;
}

BOOL MindEyeListener::operator>(const MindEyeListenerInfo &info) const
{
    return ID > info.ID;
}

BOOL MindEyeListener::operator==(const MindEyeListener &listener) const
{
    return ID == listener.ID;
}

BOOL MindEyeListener::operator<(const MindEyeListener &listener) const
{
    return ID < listener.ID;
}

BOOL MindEyeListener::operator>(const MindEyeListener &listener) const
{
    return ID < listener.ID;
}

MindEyeListener::~MindEyeListener()
{
}
