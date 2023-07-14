#pragma once
#include "Common.h"

template <typename APOClass, typename ParameterClass>
class MindEyeRendererBase : public CXAPOParametersBase
{
private:
	// 注册信息
	static XAPO_REGISTRATION_PROPERTIES m_regProps;

	// 参数
	ParameterClass m_parameters[3];

	// 波形图
	WAVEFORMATEX m_wfx;

protected:
	MindEyeRendererBase();
	~MindEyeRendererBase(void);

	void OnSetParameters(_In_reads_bytes_(cbParams) const void *pParams, UINT32 cbParams) override;
	void DoProcess(
		const ParameterClass &params,
		_Inout_updates_all_(cFrames *cChannels) FLOAT32 *__restrict pData,
		UINT32 cFrames,
		UINT32 cChannels) override;

public:
	static HRESULT CreateInstance(void *pInitData, UINT32 cbInitData, APOClass **ppAPO);

	STDMETHOD(LockForProcess)
	(UINT32 InputLockedParameterCount,
	 _In_reads_opt_(InputLockedParameterCount) const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS *pInputLockedParameters,
	 UINT32 OutputLockedParameterCount,
	 _In_reads_opt_(OutputLockedParameterCount) const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS *pOutputLockedParameters) override;

	STDMETHOD_(void, Process)
	(UINT32 InputProcessParameterCount,
	 _In_reads_opt_(InputProcessParameterCount) const XAPO_PROCESS_BUFFER_PARAMETERS *pInputProcessParameters,
	 UINT32 OutputProcessParameterCount,
	 _Inout_updates_opt_(OutputProcessParameterCount) XAPO_PROCESS_BUFFER_PARAMETERS *pOutputProcessParameters,
	 BOOL IsEnabled) override;
};

template <typename APOClass, typename ParameterClass>
inline MindEyeRendererBase<APOClass, ParameterClass>::MindEyeRendererBase() : CXAPOParametersBase(&m_regProps, (BYTE *)m_parameters, sizeof(ParameterClass), FALSE)
{
	ZeroMemory(m_parameters, sizeof(m_parameters));
}

template <typename APOClass, typename ParameterClass>
inline MindEyeRendererBase<APOClass, ParameterClass>::~MindEyeRendererBase(void)
{
}

template <typename APOClass, typename ParameterClass>
inline void MindEyeRendererBase<APOClass, ParameterClass>::OnSetParameters(const void *pParams, UINT32 cbParams)
{
}

template <typename APOClass, typename ParameterClass>
inline void MindEyeRendererBase<APOClass, ParameterClass>::DoProcess(const ParameterClass &params, FLOAT32 *__restrict pData, UINT32 cFrames, UINT32 cChannels)
{
}

/**
 * 创建音频效果实例.
 *
 * \param pInitData
 * \param cbInitData
 * \param ppAPO：待创建实例
 * \return
 */
template <typename APOClass, typename ParameterClass>
inline HRESULT MindEyeRendererBase<APOClass, ParameterClass>::CreateInstance(void *pInitData, UINT32 cbInitData, APOClass **ppAPO)
{
	_ASSERT(ppAPO);
	HRESULT hr = S_OK;

	*ppAPO = new APOClass;
	if (*ppAPO)
	{
		hr = (*ppAPO)->Initialize(pInitData, cbInitData);
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}
	return hr;
}

template <typename APOClass, typename ParameterClass>
inline STDMETHODIMP_(HRESULT __stdcall) MindEyeRendererBase<APOClass, ParameterClass>::LockForProcess(UINT32 InputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS *pInputLockedParameters, UINT32 OutputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS *pOutputLockedParameters)
{
	HRESULT hr = CXAPOParametersBase::LockForProcess(
		InputLockedParameterCount,
		pInputLockedParameters,
		OutputLockedParameterCount,
		pOutputLockedParameters);

	if (SUCCEEDED(hr))
	{
		if (!pInputLockedParameters)
			return E_POINTER;

		// 复制波形图属性
		memcpy_s(&m_wfx, sizeof(WAVEFORMATEX), pInputLockedParameters[0].pFormat, sizeof(WAVEFORMATEX));
	}
	return hr;
}

template <typename APOClass, typename ParameterClass>
inline STDMETHODIMP_(void __stdcall) MindEyeRendererBase<APOClass, ParameterClass>::Process(UINT32 InputProcessParameterCount, const XAPO_PROCESS_BUFFER_PARAMETERS *pInputProcessParameters, UINT32 OutputProcessParameterCount, XAPO_PROCESS_BUFFER_PARAMETERS *pOutputProcessParameters, BOOL IsEnabled)
{
	_ASSERT(IsLocked());
	_ASSERT(InputProcessParameterCount == 1);
	_ASSERT(OutputProcessParameterCount == 1);
	_ASSERT(pInputProcessParameters != nullptr && pOutputProcessParameters != nullptr);
	_Analysis_assume_(pInputProcessParameters != nullptr && pOutputProcessParameters != nullptr);
	_ASSERT(pInputProcessParameters[0].pBuffer == pOutputProcessParameters[0].pBuffer);

	ParameterClass *pParams;
	pParams = (ParameterClass *)BeginProcess();
	EndProcess();
}

template <typename APOClass, typename ParameterClass>
__declspec(selectany) XAPO_REGISTRATION_PROPERTIES MindEyeRendererBase<APOClass, ParameterClass>::m_regProps = {
	__uuidof(APOClass),
	L"SampleAPO",
	L"Copyright (C)2023 LaoQiongGui",
	1,
	0,
	XAPO_FLAG_INPLACE_REQUIRED | XAPO_FLAG_CHANNELS_MUST_MATCH | XAPO_FLAG_FRAMERATE_MUST_MATCH | XAPO_FLAG_BITSPERSAMPLE_MUST_MATCH | XAPO_FLAG_BUFFERCOUNT_MUST_MATCH | XAPO_FLAG_INPLACE_SUPPORTED,
	1, 1, 1, 1};