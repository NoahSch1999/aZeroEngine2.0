#include "Core/Engine.h"

extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 614; }

extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; }

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug> spDebugController0;
	Microsoft::WRL::ComPtr<ID3D12Debug1> spDebugController1;
	D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0));

	// TODO - Crashes when called
	//spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1));
	//spDebugController1->SetEnableGPUBasedValidation(true);

	Microsoft::WRL::ComPtr<ID3D12Debug> d3d12Debug;
	Microsoft::WRL::ComPtr<IDXGIDebug> idxgiDebug;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12Debug))))
		d3d12Debug->EnableDebugLayer();
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&idxgiDebug));
#endif // DEBUG

	{

		// Setup engine
		DXM::Vector2 Resolution(1920, 1080);
		aZero::Engine engine(instance, Resolution);
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

		std::shared_ptr<aZero::Window> activeWindow(engine.GetMainWindow());

		// Run engine
		while (activeWindow->IsOpen())
		{
			if (GetAsyncKeyState(VK_ESCAPE))
			{
				break;
			}

			engine.BeginFrame();

			engine.Update();

			engine.EndFrame();
		}

	}

#ifdef _DEBUG
	idxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL));
#endif // DEBUG

	return 0;
}