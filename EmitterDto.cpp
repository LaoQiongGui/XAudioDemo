#include "pch.h"
#include "EmitterDto.h"

BOOL EmitterDto::operator==(const EmitterDto &emitterDto) const
{
    return UID == emitterDto.UID;
}

BOOL EmitterDto::operator<(const EmitterDto &emitterDto) const
{
    return UID < emitterDto.UID;
}

BOOL EmitterDto::operator>(const EmitterDto &emitterDto) const
{
    return UID > emitterDto.UID;
}

std::set<EmitterDto> operator-(
    const std::set<EmitterDto> set1,
    const std::set<EmitterDto> set2)
{
    std::set<EmitterDto> resultSet = std::set<EmitterDto>();
    auto pEmitterDto1 = set1.begin();
    auto pEmitterDto2 = set2.begin();
    while (
        pEmitterDto1 != set1.end() &&
        pEmitterDto2 != set2.end())
    {
        if (*pEmitterDto1 == *pEmitterDto2)
        {
            pEmitterDto1++;
            pEmitterDto2++;
        }
        else if (*pEmitterDto1 < *pEmitterDto2)
        {
            resultSet.insert(*pEmitterDto1);
            pEmitterDto1++;
        }
        else
        {
            pEmitterDto2++;
        }
    }

    while (pEmitterDto1 != set1.end()) {
        resultSet.insert(*pEmitterDto1);
        pEmitterDto1++;
    }
    return resultSet;
}
