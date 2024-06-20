#include "..\Public\TargetManager.h"
#include "RenderTarget.h"
#include "VIBufferRect.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CTargetManager)

CTargetManager::CTargetManager()
{
}



HRESULT CTargetManager::AddRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor)
{
	if (nullptr != Find_RenderTarget(pTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(pDevice, pContext, iSizeX, iSizeY, eFormat, pClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTargetManager::AddMRT(const _tchar* pMRTTag, const _tchar* pTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>* pMRTList = FindMRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*> MRTList;

		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	SafeAddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTargetManager::BindSRV(const _tchar* pTargetTag, CShader* pShader, const char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->BindSRV(pShader, pConstantName);
}

HRESULT CTargetManager::BeginMRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDepthStencil)
{
	list<CRenderTarget*>* pMRTList = FindMRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	_uint iNumViews = 8;
	pContext->OMGetRenderTargets(iNumViews, m_pOldRenderTargets, &m_pOldDepthStencil);

	_uint iNumRTVs = 0;

	ID3D11RenderTargetView* RTVs[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		RTVs[iNumRTVs++] = pRenderTarget->GetRTV();

		if (pDepthStencil)
			pContext->ClearDepthStencilView(pDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	ID3D11DepthStencilView* pSelectDepthStencil = pDepthStencil ? pDepthStencil : m_pOldDepthStencil;

	pContext->OMSetRenderTargets(iNumRTVs, RTVs, pSelectDepthStencil);

	return S_OK;
}

HRESULT CTargetManager::BeginOverwriteMRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDepthStencil) {
	list<CRenderTarget*>* pMRTList = FindMRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	_uint iNumViews = 8;
	pContext->OMGetRenderTargets(iNumViews, m_pOldRenderTargets, &m_pOldDepthStencil);

	_uint iNumRTVs = 0;

	ID3D11RenderTargetView* RTVs[8] = { nullptr };
	for (auto& pRenderTarget : *pMRTList) {
		RTVs[iNumRTVs++] = pRenderTarget->GetRTV();
	}

	ID3D11DepthStencilView* pSelectDepthStencil = pDepthStencil ? pDepthStencil : m_pOldDepthStencil;

	pContext->OMSetRenderTargets(iNumRTVs, RTVs, pSelectDepthStencil);

	return S_OK;
}


HRESULT CTargetManager::EndMRT(ID3D11DeviceContext* pContext)
{
	_uint		iNumRTVs = 8;

	pContext->OMSetRenderTargets(iNumRTVs, m_pOldRenderTargets, m_pOldDepthStencil);

	for (_uint i = 0; i < 8; ++i)
		SafeRelease(m_pOldRenderTargets[i]);

	SafeRelease(m_pOldDepthStencil);

	return S_OK;
}

HRESULT CTargetManager::ClearSRV(ID3D11DeviceContext* pContext)
{
	ID3D11ShaderResourceView* pSRVs[8] = {
		nullptr,
	};

	pContext->PSSetShaderResources(0, 8, pSRVs);

	return S_OK;
}

ID3D11DepthStencilView* CTargetManager::CreateDepthStencil(ID3D11Device* pDevice, _uint iWinCX, _uint iWinCY)
{
	if (nullptr == pDevice)
		return nullptr;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return nullptr;

	ID3D11DepthStencilView* pNewDepthStencilView;
	if (FAILED(pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &pNewDepthStencilView)))
		return nullptr;

	SafeRelease(pDepthStencilTexture);

	return pNewDepthStencilView;
}

D3D11_VIEWPORT CTargetManager::CreateViewPort(_uint iWinCX, _uint iWinCY)
{
	D3D11_VIEWPORT ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = static_cast<FLOAT>(iWinCX);
	ViewPortDesc.Height = static_cast<FLOAT>(iWinCY);
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	return ViewPortDesc;

}

#ifdef _DEBUG

HRESULT CTargetManager::AddDebugTarget(const _tchar* pTargetTag, _uint iCol, _uint iRow)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);

	const _float fCellSizeX = 100.f;
	const _float fCellSizeY = 100.f;
	const _float startX = fCellSizeX * 0.5f;
	const _float startY = fCellSizeY * 0.5f;

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->InitializeDebug(startX + (iRow * fCellSizeX), startY + (iCol * fCellSizeY), fCellSizeX, fCellSizeY);
}

HRESULT CTargetManager::InitializeDebug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->InitializeDebug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTargetManager::AddDebugMRT(const _tchar* pMRTTag)
{
	list<CRenderTarget*>* pMRTList = FindMRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	m_DebugMRTs.push_back(pMRTList);

	return S_OK;
}

HRESULT CTargetManager::RenderDebug(class CVIBuffer* pVIBuffer, class CShader* pShader)
{
	//list<CRenderTarget*>* pMRTList = FindMRT(pMRTTag);
	//if (nullptr == pMRTList)
	//	return E_FAIL;

	//for (auto& pRenderTarget : *pMRTList)
	//{
	//	pRenderTarget->RenderDebug(pShader, pVIBuffer);
	//}

	for (auto& MRT : m_DebugMRTs) {
		for (auto& pRenderTarget : *MRT) {
			pRenderTarget->RenderDebug(pShader, pVIBuffer);
		}
	}

	return S_OK;
}

#endif // _DEBUG

CRenderTarget* CTargetManager::Find_RenderTarget(const _tchar* pTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTagFinder(pTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTargetManager::FindMRT(const _tchar* pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTagFinder(pMRTTag));
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTargetManager::Free()
{


	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			SafeRelease(pRenderTarget);

		Pair.second.clear();
	}

	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		SafeRelease(Pair.second);

	m_RenderTargets.clear();

}
