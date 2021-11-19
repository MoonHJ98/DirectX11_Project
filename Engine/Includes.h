#pragma once

#ifdef max
#undef max
#undef min
#endif

#define VSYNC_ENABLE true
#define FULLSCREEN false
#define SCREENSIZEX 800
#define SCREENSIZEY 600

#define SCREENDEPTH 1000.f
#define SCREENNEAR 0.1f
#define FoV 3.141592654f / 4.f

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dtypes.h>
#include <d3dcompiler.h>
#include <memory>
#include <wrl.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <fstream>
#include <iostream>
#include <stdio.h>

#include <DirectXTK/BufferHelpers.h>
#include <DirectXTK/SimpleMath.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/GeometricPrimitive.h>
#include "DirectXTex.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma comment(lib, "assimp-vc140-mt.lib")
#include <d3dcompiler.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "DirectXTex.lib")
//#pragma comment(lib, "assimp-vc141-mtd.lib")



using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace SimpleMath;
using namespace PackedVector;


#define MSG_BOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("System Message"), MB_OK)
#define SINGLETON(CLASSNAME)													\
			private:																	\
			CLASSNAME() {}																\
			CLASSNAME(const CLASSNAME& Rhs) = delete;									\
			CLASSNAME& operator=(const CLASSNAME& Rhs) = delete;						\
			public:																		\
			static shared_ptr<CLASSNAME> GetInstance()									\
			{																			\
					static shared_ptr<CLASSNAME> instance{ new CLASSNAME};				\
					return instance;													\
			};																						

#define SAFEDELETE(Instance) { if(Instance) {delete Instance; Instance = nullptr;}}
#define SAFEDELETEARRAY(Instance){ if(Instance){ delete [] Instance; Instance = nullptr; } }

#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500


#include "Struct.h"