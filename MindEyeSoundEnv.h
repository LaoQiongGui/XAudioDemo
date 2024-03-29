#pragma once
#include "Common.h"
#include "MindEyeDto.h"
#include "MindEyeRenderer2D.h"

class MindEyeSoundEnv
{
private:
	// 普通变量
	// 声源音频格式
	WAVEFORMATEX WFXSrc;
	// 输出音频格式
	WAVEFORMATEX WFXOut;
	// XAudio引擎
	IXAudio2 *pXAudio2;
	// 主音频
	IXAudio2MasteringVoice *pMasterVoice;
	// 左声道
	IXAudio2SubmixVoice *pSubmixVoiceLeft;
	// 右声道
	IXAudio2SubmixVoice *pSubmixVoiceRight;
	// 音频发生器
	std::map<DWORD64, MindEyeEmitter *> emitterMap;
	// 音频接收器
	std::map<DWORD64, MindEyeListener *> listenerMap;
	// FFT计划
	fftw_plan fftPlan;
	// IFFT计划
	fftw_plan ifftPlan;
	// 时域数据
	double *pDataTD;
	// 频域数据
	fftw_complex *pDataFD;

	// 函数指针
	// 获取音频发生器信息
	std::function<std::map<DWORD64, MindEyeEmitterInfo *>(void)> getEmitterInfo;
	// 获取音频接收器信息
	std::function<std::map<DWORD64, MindEyeListenerInfo *>(void)> getListenerInfo;

protected:
public:
	// 构造声音环境
	MindEyeSoundEnv(
		IXAudio2 *pXAudio2, IXAudio2MasteringVoice *pMasterVoice, WAVEFORMATEX WFXOut,
		std::function<std::map<DWORD64, MindEyeEmitterInfo *>(void)> getEmitterInfo,
		std::function<std::map<DWORD64, MindEyeListenerInfo *>(void)> getListenerInfo);
	~MindEyeSoundEnv();

	// 刷新声音环境
	HRESULT OnRefresh();
	// 获取环境复杂度，用于多线程优化
	DWORD getEnvComplexity();
};