#pragma once

/**
 * 空间信息.
 */
struct SpaceInfo
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

/**
 * 发生器信息.
 */
struct EmitterInfoDto
{
	DWORD64 ID;
	// 空间信息
	SpaceInfo spaceInfo;
	// 音频缓冲区
	UINT32 bufferSize;
	BYTE* pBufferData;

	BOOL operator==(const EmitterInfoDto& emitterDto) const;
	BOOL operator<(const EmitterInfoDto& emitterDto) const;
	BOOL operator>(const EmitterInfoDto& emitterDto) const;

	// 单UID构造函数
	EmitterInfoDto(const DWORD64 UID);
};

/**
 * 接收器信息.
 */
struct ListenerInfoDto
{
	DWORD64 ID;
	// 空间信息
	SpaceInfo spaceInfo;

	BOOL operator==(const ListenerInfoDto& listenerDto) const;
	BOOL operator<(const ListenerInfoDto& listenerDto) const;
	BOOL operator>(const ListenerInfoDto& listenerDto) const;
};
