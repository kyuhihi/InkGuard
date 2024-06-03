#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CResourceLoader final : public CBase
{
public:
	enum RESOURCE_TYPE 
	{
		TYPE_TEXTURE,
		TYPE_MODEL,
		TYPE_CALL,
		TYPE_END
	};

	using RESOURCE_INFO = struct tagResourceInfo
	{
		RESOURCE_TYPE eType;
		_tchar szWidePrototypeTag[MAX_PATH] = TEXT("");
	};

	using TEXTURE_INFO = struct tagTextureInfo : public RESOURCE_INFO
	{
		_tchar szWideTextureFilePath[MAX_PATH] = TEXT("");
		_uint iNumTexture = 0;
	};

	enum MODEL_LOAD_TYPE
	{
		MODEL_LOAD_ASSIMP,
		MODEL_LOAD_BIN,
		MODEL_LOAD_END
	};

	using MODEL_INFO = struct tagModelInfo : public RESOURCE_INFO 
	{
		char strFilePath[MAX_PATH] = "";
		_tchar wFilePath[MAX_PATH] = TEXT("");

		_uint iModelType = 0;
		_float3 fRadian = { 0.f, 0.f, 0.f };
		_float3 vScale = { 0.f, 0.f, 0.f };

		_uint iFlag = 0;

		_float4x4 PivotMatrix;

		MODEL_LOAD_TYPE eLoadType;
	};

	using CALL_INFO = struct tagCallInfo : public RESOURCE_INFO
	{
		function<_bool()> pCall;
	};

private:
	CResourceLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumLevel);
	virtual ~CResourceLoader() = default;

public:
	virtual HRESULT Initialize();

public:
	HRESULT JsonParsing(const char* pJsonFilePath);
	HRESULT LoadJson(const char* pJsonFilePath);
	HRESULT Loading(_uint iIndex);

	HRESULT EnqueueModel(string& ResourceType, Value& value);
	HRESULT EnqueueModel(const _tchar* pPrototypeTag, const char* pModelFilePath, MODEL_LOAD_TYPE eLoadType, _uint iModelType, _matrix pivotMatrix, _uint iFlag = 0);

	HRESULT EnqueueTexture(string& ResourceType, Value& value);
	HRESULT EnqueueTexture(const _tchar* pPrototypeTag, const _tchar* pTextureFilePath, _uint iNumTexture = 1);

	HRESULT EnqueueCall(function<bool()> pCall);

	HRESULT ModelCreate(MODEL_INFO* pInfo);
	HRESULT TextureCreate(TEXTURE_INFO* pInfo);
	HRESULT CallCreate(CALL_INFO* pInfo);

	void ToolTick(_float fTimeDelta);

	vector<string> GetRecentLog(_uint iShowNum);

private:
	void RecordLog(RESOURCE_INFO* resourceInfo, string strLog = "");

public:
	const _uint GetLoadSize() const { return static_cast<_uint>(m_LoadList.size()); }
	const _uint GetLoadCount() const { return m_iLoadCount; }

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	_uint m_iNumLevel;
	_uint m_iLoadCount;
	mutex m_Lock;
	_bool m_bLogPushed;

private:
	Document m_Document;

private:
	vector<RESOURCE_INFO*> m_LoadList;
	vector<string> m_strLoadLog;

	_uint m_iLogSIze;

public:
	static CResourceLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumLevel);
	virtual void Free();
};

END