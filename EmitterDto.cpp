#include "pch.h"
#include "EmitterDto.h"

BOOL EmitterDto::operator==(const EmitterDto& emitterDto) const
{
    return UID == emitterDto.UID;
}

BOOL EmitterDto::operator<(const EmitterDto& emitterDto) const
{
    return UID < emitterDto.UID;
}

BOOL EmitterDto::operator>(const EmitterDto& emitterDto) const
{
    return UID > emitterDto.UID;
}
