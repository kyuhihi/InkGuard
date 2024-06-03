#pragma once

#include "MeshContainer.h"

BEGIN(Engine)
class CMeshContainerEx final : public CMeshContainer
{
private:
	CMeshContainerEx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainerEx(const CMeshContainerEx& rhs);
	virtual ~CMeshContainerEx() = default;

public:
	virtual HRESULT Save(HANDLE& hFile, _ulong& dwByte) override;
	virtual HRESULT Load(HANDLE& hFile, _ulong& dwByte, CModel::TYPE eModelType, _fmatrix pivotMatrix) override;

private:
	virtual HRESULT ReadyVertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix) override;
	virtual HRESULT ReadyAnimVertices(const aiMesh* pAIMesh, CModel* pModel) override;

private:
	VTXMODELEX* m_pEXVertices;
	VTXANIMMODELEX* m_pEXAnimVertices;

public:
	static CMeshContainerEx* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	static CMeshContainerEx* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END