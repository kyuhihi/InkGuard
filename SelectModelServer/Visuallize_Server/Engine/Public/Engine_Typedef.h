#pragma once

namespace Engine
{
	enum DIMK { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_X, DIMK_END };
	enum DIMM { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };
	enum SOUND_CHANNEL {
		SOUND_CHANNEL_EFFECT,
		SOUND_CHANNEL_ONEEFFECT,
		SOUND_CHANNEL_LOOPEFFECT,
		SOUND_CHANNEL_BGM,
		SOUND_CHANNEL_MONSTER,
		SOUND_CHANNEL_END
	};

	using _uchar = unsigned char;
	using _char = signed char;

	using _ushort = unsigned short;
	using _short = signed short;
		
	using _ulong = unsigned long;
	using _long = signed long;

	using _uint = unsigned int;
	using _int = signed int;

	using _float = float;
	using _double = double;

	using _bool = bool;
	using _tchar = wchar_t;

	using _float2 = XMFLOAT2;
	using _float3 = XMFLOAT3;
	using _float4 = XMFLOAT4;
	using _vector = XMVECTOR;
	using _fvector = FXMVECTOR;

	using _float4x4 = XMFLOAT4X4;
	using _matrix = XMMATRIX;
	using _fmatrix = FXMMATRIX;
}