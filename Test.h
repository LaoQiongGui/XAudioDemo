#pragma once
#include "Common.h"
#include "MindEyeDto.h"

HRESULT testInit();

std::set<MindEyeEmitterInfo> getEmitterInfoForTest();

std::set<MindEyeListenerInfo> getListenerInfoForTest();