// ======================================================================================
//  Windows SDK XAPOBase 実装ファイル
//  (xapobase.h の実体部分。Windows SDK では lib として提供されないため自前実装が必要)
// ======================================================================================

#include "xapobase.h"
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <new>
#include <assert.h>

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (P)
#endif

// ======================================================================================
//  CXAPOBase
// ======================================================================================

CXAPOBase::CXAPOBase(const XAPO_REGISTRATION_PROPERTIES* pRegistrationProperties)
    : m_pRegistrationProperties(pRegistrationProperties),
    m_pfnMatrixMixFunction(nullptr),
    m_pfl32MatrixCoefficients(nullptr),
    m_nSrcFormatType(0),
    m_fIsScalarMatrix(FALSE),
    m_fIsLocked(FALSE),
    m_lReferenceCount(1)
{}

CXAPOBase::~CXAPOBase()
{}

HRESULT CXAPOBase::ValidateFormatDefault(WAVEFORMATEX* pFormat, BOOL fOverwrite)
{
    if (!pFormat)
        return E_INVALIDARG;

    if (pFormat->wFormatTag != XAPOBASE_DEFAULT_FORMAT_TAG)
    {
        if (!fOverwrite)
            return XAPO_E_FORMAT_UNSUPPORTED;

        pFormat->wFormatTag = XAPOBASE_DEFAULT_FORMAT_TAG;
        pFormat->wBitsPerSample = XAPOBASE_DEFAULT_FORMAT_BITSPERSAMPLE;
        pFormat->nBlockAlign = (pFormat->nChannels * pFormat->wBitsPerSample) / 8;
        pFormat->nAvgBytesPerSec = pFormat->nSamplesPerSec * pFormat->nBlockAlign;
    }

    if (pFormat->nChannels < XAPOBASE_DEFAULT_FORMAT_MIN_CHANNELS ||
        pFormat->nChannels > XAPOBASE_DEFAULT_FORMAT_MAX_CHANNELS)
    {
        if (!fOverwrite)
            return XAPO_E_FORMAT_UNSUPPORTED;

        pFormat->nChannels = (BYTE)std::min<UINT32>(XAPOBASE_DEFAULT_FORMAT_MAX_CHANNELS,
            std::max<UINT32>(XAPOBASE_DEFAULT_FORMAT_MIN_CHANNELS, pFormat->nChannels));
    }

    if (pFormat->nSamplesPerSec < XAPOBASE_DEFAULT_FORMAT_MIN_FRAMERATE ||
        pFormat->nSamplesPerSec > XAPOBASE_DEFAULT_FORMAT_MAX_FRAMERATE)
    {
        if (!fOverwrite)
            return XAPO_E_FORMAT_UNSUPPORTED;

        pFormat->nSamplesPerSec = std::min<UINT32>(
            XAPOBASE_DEFAULT_FORMAT_MAX_FRAMERATE,
            std::max<UINT32>(XAPOBASE_DEFAULT_FORMAT_MIN_FRAMERATE, pFormat->nSamplesPerSec));
    }

    return S_OK;
}

HRESULT CXAPOBase::ValidateFormatPair(const WAVEFORMATEX* pSupportedFormat, WAVEFORMATEX* pRequestedFormat, BOOL fOverwrite)
{
    if (!pRequestedFormat || !pSupportedFormat)
        return E_INVALIDARG;

    HRESULT hr = ValidateFormatDefault(pRequestedFormat, fOverwrite);
    if (FAILED(hr))
        return hr;

    if ((pRequestedFormat->nChannels != pSupportedFormat->nChannels) ||
        (pRequestedFormat->nSamplesPerSec != pSupportedFormat->nSamplesPerSec) ||
        (pRequestedFormat->wBitsPerSample != pSupportedFormat->wBitsPerSample))
    {
        if (!fOverwrite)
            return XAPO_E_FORMAT_UNSUPPORTED;

        memcpy(pRequestedFormat, pSupportedFormat, sizeof(WAVEFORMATEX));
    }

    return S_OK;
}

HRESULT CXAPOBase::GetRegistrationProperties(XAPO_REGISTRATION_PROPERTIES** ppRegistrationProperties)
{
    if (!ppRegistrationProperties)
        return E_INVALIDARG;

    *ppRegistrationProperties = (XAPO_REGISTRATION_PROPERTIES*)XAPOAlloc(sizeof(XAPO_REGISTRATION_PROPERTIES));
    if (!*ppRegistrationProperties)
        return E_OUTOFMEMORY;

    memcpy(*ppRegistrationProperties, m_pRegistrationProperties, sizeof(XAPO_REGISTRATION_PROPERTIES));
    return S_OK;
}

HRESULT CXAPOBase::IsInputFormatSupported(
    const WAVEFORMATEX* pOutputFormat,
    const WAVEFORMATEX* pRequestedInputFormat,
    WAVEFORMATEX** ppSupportedInputFormat)
{
    UNREFERENCED_PARAMETER(pOutputFormat);

    if (!pRequestedInputFormat)
        return E_INVALIDARG;

    HRESULT hr = ValidateFormatDefault(const_cast<WAVEFORMATEX*>(pRequestedInputFormat), FALSE);
    if (FAILED(hr))
    {
        if (ppSupportedInputFormat)
        {
            *ppSupportedInputFormat = (WAVEFORMATEX*)XAPOAlloc(sizeof(WAVEFORMATEX));
            if (*ppSupportedInputFormat)
            {
                memcpy(*ppSupportedInputFormat, pRequestedInputFormat, sizeof(WAVEFORMATEX));
                ValidateFormatDefault(*ppSupportedInputFormat, TRUE);
            }
        }
        return XAPO_E_FORMAT_UNSUPPORTED;
    }

    if (ppSupportedInputFormat)
        *ppSupportedInputFormat = nullptr;

    return S_OK;
}

HRESULT CXAPOBase::IsOutputFormatSupported(
    const WAVEFORMATEX* pInputFormat,
    const WAVEFORMATEX* pRequestedOutputFormat,
    WAVEFORMATEX** ppSupportedOutputFormat)
{
    UNREFERENCED_PARAMETER(pInputFormat);

    if (!pRequestedOutputFormat)
        return E_INVALIDARG;

    HRESULT hr = ValidateFormatDefault(const_cast<WAVEFORMATEX*>(pRequestedOutputFormat), FALSE);
    if (FAILED(hr))
    {
        if (ppSupportedOutputFormat)
        {
            *ppSupportedOutputFormat = (WAVEFORMATEX*)XAPOAlloc(sizeof(WAVEFORMATEX));
            if (*ppSupportedOutputFormat)
            {
                memcpy(*ppSupportedOutputFormat, pRequestedOutputFormat, sizeof(WAVEFORMATEX));
                ValidateFormatDefault(*ppSupportedOutputFormat, TRUE);
            }
        }
        return XAPO_E_FORMAT_UNSUPPORTED;
    }

    if (ppSupportedOutputFormat)
        *ppSupportedOutputFormat = nullptr;

    return S_OK;
}

HRESULT CXAPOBase::LockForProcess(
    UINT32 InputLockedParameterCount,
    const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockedParameters,
    UINT32 OutputLockedParameterCount,
    const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockedParameters)
{
    UNREFERENCED_PARAMETER(InputLockedParameterCount);
    UNREFERENCED_PARAMETER(pInputLockedParameters);
    UNREFERENCED_PARAMETER(OutputLockedParameterCount);
    UNREFERENCED_PARAMETER(pOutputLockedParameters);

    m_fIsLocked = TRUE;
    return S_OK;
}

void CXAPOBase::UnlockForProcess()
{
    m_fIsLocked = FALSE;
}

// ======================================================================================
//  CXAPOParametersBase
// ======================================================================================

CXAPOParametersBase::CXAPOParametersBase(
    const XAPO_REGISTRATION_PROPERTIES* pRegistrationProperties,
    BYTE* pParameterBlocks,
    UINT32 ParameterBlockByteSize,
    BOOL fProducer)
    : CXAPOBase(pRegistrationProperties),
    m_pParameterBlocks(pParameterBlocks),
    m_uParameterBlockByteSize(ParameterBlockByteSize),
    m_fProducer(fProducer),
    m_pCurrentParameters(nullptr),
    m_pCurrentParametersInternal(nullptr),
    m_uCurrentParametersIndex(0),
    m_fNewerResultsReady(FALSE)
{
    if (pParameterBlocks && ParameterBlockByteSize > 0)
    {
        memset(pParameterBlocks, 0, ParameterBlockByteSize * 3);
        m_pCurrentParameters = pParameterBlocks;
        m_pCurrentParametersInternal = pParameterBlocks;
    }
}

CXAPOParametersBase::~CXAPOParametersBase()
{}

void CXAPOParametersBase::SetParameters(const void* pParameters, UINT32 ParameterByteSize)
{
    if (!pParameters || ParameterByteSize != m_uParameterBlockByteSize)
        return;

    BYTE* dst = m_pParameterBlocks + ((m_uCurrentParametersIndex + 1) % 3) * m_uParameterBlockByteSize;
    memcpy(dst, pParameters, ParameterByteSize);

    m_pCurrentParameters = dst;
    m_uCurrentParametersIndex = (m_uCurrentParametersIndex + 1) % 3;
    m_fNewerResultsReady = TRUE;
}

void CXAPOParametersBase::GetParameters(void* pParameters, UINT32 ParameterByteSize)
{
    if (!pParameters || ParameterByteSize != m_uParameterBlockByteSize)
        return;

    memcpy(pParameters, m_pCurrentParametersInternal, ParameterByteSize);
}

BOOL CXAPOParametersBase::ParametersChanged()
{
    return m_fNewerResultsReady;
}

BYTE* CXAPOParametersBase::BeginProcess()
{
    m_pCurrentParametersInternal = m_pCurrentParameters;
    return m_pCurrentParametersInternal;
}

void CXAPOParametersBase::EndProcess()
{
    m_fNewerResultsReady = FALSE;
}

