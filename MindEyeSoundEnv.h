#pragma once
#include "Common.h"
#include "MindEyeEmitter.h"
#include "MindEyeListener.h"

class MindEyeSoundEnv
{
private:
	DWORD GroupID;
	// 当前最大发生器UID
	DWORD EmitterUID;
	// 当前最大接收器UID
	DWORD ListenerUID;
	WAVEFORMATEXTENSIBLE WFX;
	IXAudio2 *pXAudio2;
	IXAudio2SubmixVoice *pMasterVoice;
	IXAudio2SubmixVoice *pSubmixVoice;
	std::set<MindEyeEmitter> emitters;
	std::set<MindEyeListener> listeners;

protected:
public:
	MindEyeSoundEnv(DWORD, IXAudio2 *, IXAudio2SubmixVoice *, WAVEFORMATEXTENSIBLE);
	~MindEyeSoundEnv();

	HRESULT OnInit();
	HRESULT OnRefresh(DWORD64 dwTimeStamp);
	// 获取环境复杂度，用于多线程优化
	DWORD getEnvComplexity();

	BOOL operator==(const MindEyeSoundEnv &env) const;
	BOOL operator<(const MindEyeSoundEnv &env) const;
	BOOL operator>(const MindEyeSoundEnv &env) const;
};