#pragma once
#include "Common.h"

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
 * 外部传入发生器信息.
 */
struct MindEyeEmitterInfo
{
	// 发生器ID
	DWORD64 ID;
	// 发生器空间信息
	SpaceInfo spaceInfo;
	// 发生器缓冲区大小
	DWORD bufferSize;
	// 发生器缓冲区数据
	BYTE *pBufferData;

	BOOL operator==(const DWORD64 &ID) const;
	BOOL operator<(const DWORD64 &ID) const;
	BOOL operator>(const DWORD64 &ID) const;
	BOOL operator==(const MindEyeEmitterInfo &info) const;
	BOOL operator<(const MindEyeEmitterInfo &info) const;
	BOOL operator>(const MindEyeEmitterInfo &info) const;
};

/**
 * 内部使用发生器信息.
 */
struct MindEyeEmitter
{
	// 发生器ID
	DWORD64 ID;
	// 发生器空间信息
	SpaceInfo spaceInfo;
	// 发生器音频缓冲区
	XAUDIO2_BUFFER buffer;
	// 左声道
	IXAudio2SourceVoice *pAudio2SourceVoiceL;
	XAUDIO2_SEND_DESCRIPTOR sendL;
	XAUDIO2_VOICE_SENDS sendlistL;
	XAUDIO2_EFFECT_CHAIN effectChainL;
	// 右声道
	IXAudio2SourceVoice *pAudio2SourceVoiceR;
	XAUDIO2_SEND_DESCRIPTOR sendR;
	XAUDIO2_VOICE_SENDS sendlistR;
	XAUDIO2_EFFECT_CHAIN effectChainR;

	MindEyeEmitter() = default;
	~MindEyeEmitter();

	BOOL operator==(const DWORD64 &ID) const;
	BOOL operator<(const DWORD64 &ID) const;
	BOOL operator>(const DWORD64 &ID) const;
	BOOL operator==(const MindEyeEmitterInfo &info) const;
	BOOL operator<(const MindEyeEmitterInfo &info) const;
	BOOL operator>(const MindEyeEmitterInfo &info) const;
	BOOL operator==(const MindEyeEmitter &emitter) const;
	BOOL operator<(const MindEyeEmitter &emitter) const;
	BOOL operator>(const MindEyeEmitter &emitter) const;
};

/**
 * 外部传入接收器信息.
 */
struct MindEyeListenerInfo
{
	// 接收器ID
	DWORD64 ID;
	// 空间信息
	SpaceInfo spaceInfo;

	BOOL operator==(const DWORD64 &ID) const;
	BOOL operator<(const DWORD64 &ID) const;
	BOOL operator>(const DWORD64 &ID) const;
	BOOL operator==(const MindEyeListenerInfo &info) const;
	BOOL operator<(const MindEyeListenerInfo &info) const;
	BOOL operator>(const MindEyeListenerInfo &info) const;
};

/**
 * 内部使用接收器信息.
 */
struct MindEyeListener
{
	// 接收器ID
	DWORD64 ID;
	// 空间信息
	SpaceInfo spaceInfo;

	BOOL operator==(const DWORD64 &ID) const;
	BOOL operator<(const DWORD64 &ID) const;
	BOOL operator>(const DWORD64 &ID) const;
	BOOL operator==(const MindEyeListenerInfo &info) const;
	BOOL operator<(const MindEyeListenerInfo &info) const;
	BOOL operator>(const MindEyeListenerInfo &info) const;
	BOOL operator==(const MindEyeListener &listener) const;
	BOOL operator<(const MindEyeListener &listener) const;
	BOOL operator>(const MindEyeListener &listener) const;

	~MindEyeListener();
};
