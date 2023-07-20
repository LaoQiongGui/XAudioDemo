#pragma once
#include "Common.h"
#include "MindEyeDto.h"
#include "MindEyeRenderer2D.h"

class MindEyeSoundEnv
{
private:
	// 普通变量
	// 声音环境UID
	DWORD GroupID;
	// 当前最大发生器UID
	DWORD EmitterUID;
	// 当前最大接收器UID
	DWORD ListenerUID;
	// 主音频格式
	WAVEFORMATEXTENSIBLE WFX;
	// XAudio引擎
	IXAudio2 *pXAudio2;
	// 主音频
	IXAudio2SubmixVoice *pMasterVoice;
	// 左声道
	IXAudio2SubmixVoice *pSubmixVoiceLeft;
	// 右声道
	IXAudio2SubmixVoice *pSubmixVoiceRight;
	// 音频发生器
	std::set<MindEyeEmitter> emitters;
	// 音频接收器
	std::set<MindEyeListener> listeners;

	// 函数指针
	// 获取音频发生器信息
	std::set<MindEyeEmitterInfo> (*getEmitterInfo)();
	// 获取音频接收器信息
	std::set<MindEyeListenerInfo> (*getListenerInfo)();

protected:
public:
	// 构造声音环境
	MindEyeSoundEnv(
		DWORD GroupIDIn, IXAudio2 *pXAudio2In, IXAudio2SubmixVoice *pMasterVoiceIn, WAVEFORMATEXTENSIBLE WFXIn,
		std::set<MindEyeEmitterInfo> (*getEmitterInfoIn)(), std::set<MindEyeListenerInfo> (*getListenerInfo)());
	~MindEyeSoundEnv();

	// 刷新声音环境
	HRESULT OnRefresh();
	// 获取环境复杂度，用于多线程优化
	DWORD getEnvComplexity();

	BOOL operator==(const MindEyeSoundEnv &env) const;
	BOOL operator<(const MindEyeSoundEnv &env) const;
	BOOL operator>(const MindEyeSoundEnv &env) const;
};