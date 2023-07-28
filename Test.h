#pragma once
#include "Common.h"
#include "MindEyeDto.h"

HRESULT testInit();

std::map<DWORD64, MindEyeEmitterInfo*> getEmitterInfoForTest();

std::map<DWORD64, MindEyeListenerInfo*> getListenerInfoForTest();
