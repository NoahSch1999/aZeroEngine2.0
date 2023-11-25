#pragma once
#include <Windows.h>
#include <DirectX-Headers/include/directx/d3d12.h>
#include <DirectX-Headers/include/directx/d3dx12.h>
#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <DirectXTK12/Inc/SimpleMath.h>

namespace DXM = DirectX::SimpleMath;

#ifdef _DEBUG
#include <dxgidebug.h>
#endif // _DEBUG