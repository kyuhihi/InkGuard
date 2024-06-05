
#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)
#pragma warning (disable : 4996)

#ifndef NOMINMAX
# define NOMINMAX
#endif // rapidjson내 max 매크로 경고 문제 수정

#include <d3d11.h>
#include <d3dcompiler.h>
#include <FX11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include <DirectXCollision.h>

#include <fstream>

#include <Assimp/scene.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <DirectXMath.h>
using namespace DirectX;

/* Json */
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <typeinfo>
#include <functional>
#include <process.h>
#include <iostream>
#include <random>

#include <io.h> 
#include <Fmod/fmod.h>
#include <Fmod/fmod.hpp>

#ifdef _DEBUG
#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_win32.h"
#include "Imgui\imgui_impl_dx11.h"
#include "Imgui\ImGuizmo.h"
#endif

#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Functor.h"
#include "Engine_Function.h"



#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace std;
using namespace Engine;


