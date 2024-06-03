#pragma once

namespace Engine
{
	using KEYFRAME = struct tagKeyFrame
	{
		float	 fTime;
				 
		XMFLOAT3 vScale;
		XMFLOAT4 vRotation;
		XMFLOAT3 vPosition;		
	};

	/* 빛의 정보를 담아놓기 위한 구조체. */
	using LIGHTDESC = struct tagLightDesc
	{
		enum TYPE { TYPE_POINT, TYPE_DIRECTIONAL, TYPE_END };

		TYPE	 eType;
				 
		XMFLOAT4 vDirection;
				 
		XMFLOAT4 vPosition;
		float	 fRange;
				 
		XMFLOAT4 vDiffuse;
		XMFLOAT4 vAmbient;
		XMFLOAT4 vSpecular;

		float fBias;

	};

	using MATERIALDESC = struct tagMaterialDesc
	{
		class CTexture* pTexture[AI_TEXTURE_TYPE_MAX];
	};

	using LINEINDICES16 = struct tagLineIndices16
	{
		unsigned short _0, _1;
	};

	using LINEINDICES32 = struct tagLineIndices32
	{
		unsigned long _0, _1;
	};

	using FACEINDICES16 = struct tagFaceIndices16
	{
		unsigned short _0, _1, _2;
	};

	using FACEINDICES32 = struct tagFaceIndices32
	{
		unsigned long _0, _1, _2;
	};

	// D3DDECLUSAGE
	using VTXTEX = struct tagVertexTexture
	{
		XMFLOAT3 vPosition;
		XMFLOAT2 vTexture;		
	};

	using VTXTEX_DECLARATION = struct ENGINE_DLL tagVertexTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXNORTEX = struct tagVertexNormalTexture
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexture;
	};

	using VTXNORTEX_DECLARATION = struct ENGINE_DLL tagVertexNormalTexture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXMODEL = struct tagVertexModel
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexture;
		XMFLOAT3 vTangent;
	};

	using VTXMODEL_DECLARATION = struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXANIMMODEL = struct tagVertexAnimModel
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexture;
		XMFLOAT3 vTangent;
		XMUINT4	 vBlendIndex; /* 이 정점에 영향을 주는 뼈의 인덱스 네개. */
		XMFLOAT4 vBlendWeight; /* 영향르 주고 있는 각 뼈대의 영향 비율 */
	};

	using VTXANIMMODEL_DECLARATION = struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXCUBETEX = struct tagVertexCubeTexture
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vTexture;
	};

	using VTXCUBETEX_DECLARATION = struct ENGINE_DLL tagVertexCubeTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};


	using VTXCOL = struct tagVertexColor
	{
		XMFLOAT3 vPosition;
		XMFLOAT4 vColor;
	};

	using VTXCOL_DECLARATION = struct ENGINE_DLL tagVertexColor_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXINSTANCE = struct tagVertexInstance
	{		
		XMFLOAT4 vRight;
		XMFLOAT4 vUp;
		XMFLOAT4 vLook;
		XMFLOAT4 vPosition;
	};

	using VTXRECTINSTANCE_DECLARATION = struct ENGINE_DLL tagVertexRectInstance_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXPOINT = struct tagVertexPoint
	{
		XMFLOAT3 vPosition;
		XMFLOAT2 vSize;
	};

	using VTXPOINTINSTANCE_DECLARATION = struct ENGINE_DLL tagVertexPointInstance_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};


	using GRAPHICDESC = struct tagGraphicDesc
	{
		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		HWND hWnd;
		unsigned int iWinSizeX;
		unsigned int iWinSizeY;
		WINMODE eWinMode;
	};

}
