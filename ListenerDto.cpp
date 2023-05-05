#include "pch.h"
#include "ListenerDto.h"

BOOL ListenerDto::operator==(const ListenerDto &listenerDto) const
{
    return UID == listenerDto.UID;
}

BOOL ListenerDto::operator<(const ListenerDto &listenerDto) const
{
    return UID < listenerDto.UID;
}

BOOL ListenerDto::operator>(const ListenerDto &listenerDto) const
{
    return UID > listenerDto.UID;
}
