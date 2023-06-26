#pragma once

struct EmitterInfoDto
{
	DWORD64 UID;
	// 音频缓冲区
	UINT32 bufferSize;
	BYTE* pBufferData;

	BOOL operator==(const EmitterInfoDto& emitterDto) const;
	BOOL operator<(const EmitterInfoDto& emitterDto) const;
	BOOL operator>(const EmitterInfoDto& emitterDto) const;
};

struct ListenerInfoDto
{
	DWORD64 UID;

	BOOL operator==(const ListenerInfoDto& listenerDto) const;
	BOOL operator<(const ListenerInfoDto& listenerDto) const;
	BOOL operator>(const ListenerInfoDto& listenerDto) const;
};

struct PhyInfoDto
{
	// 坐标
	FLOAT PX;
	FLOAT PY;
	FLOAT PZ;
	// 速度
	FLOAT VX;
	FLOAT VY;
	FLOAT VZ;
};