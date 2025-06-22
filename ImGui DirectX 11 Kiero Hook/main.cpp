#include "includes.h"
#include <math.h>
#include "menu/Frontend/Framework/MenuFramework.h"
#include "menu/Frontend/Renderer/Renderer.h"
#include "menu/Frontend/Menu/Menu.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void gamesenserender() {
	ImGui::GetIO().MouseDrawCursor = CMenu::get()->IsMenuOpened();
	Render::Draw->Reset();

	CMenu::get()->Initialize();
	CMenu::get()->Draw();
}


void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);

			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

			DXGI_SWAP_CHAIN_DESC sc;
			pSwapChain->GetDesc(&sc);

			window = sc.OutputWindow;
			oWndProc = (WNDPROC)SetWindowLongPtrW(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);

			pBackBuffer->Release();

			ImGui::CreateContext();
			ImGui::StyleColorsDark();

			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(pDevice, pContext);

			Render::Draw->Init(pDevice, window);
			init = true;
		}
		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	for (int i = 0; i < Render::Draw->font_queue.size(); i++)
	{
		for (int c = 0; c < Render::Draw->font_queue.at(i).size(); c++)
		{
			auto& el = Render::Draw->font_queue.at(i).at(c);
			Render::Draw->fonts.at(i)[std::get<0>(el)] = ImGui::GetIO().Fonts->AddFontFromFileTTF(std::get<1>(el).c_str(), std::get<2>(el));
		}

		if (Render::Draw->font_queue.at(i).size())
			ImGui_ImplDX11_InvalidateDeviceObjects();

		Render::Draw->font_queue.at(i).clear();
	}


	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	gamesenserender();

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Allocate a new console
		AllocConsole();

		// Redirect standard input, output, and error streams to the new console
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
		freopen_s(&f, "CONOUT$", "w", stderr);
		freopen_s(&f, "CONIN$", "r", stdin);
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}
