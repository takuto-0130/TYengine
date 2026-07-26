#include "WindowsApp.h"
#include "Utils/JSON/JsonManager.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include <string>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma comment(lib, "winmm.lib")


namespace TYEngine
{
	namespace Core
	{

		LRESULT WindowsApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{

			if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
			{
				return true;
			}

			switch (msg)
			{
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			}

			return DefWindowProc(hwnd, msg, wparam, lparam);
		}

		void WindowsApp::Initialize()
		{
			HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
			(void)hr;
			assert(SUCCEEDED(hr));

			Utility::JsonManager jm;
			jm.Load("EngineConfig.json");

			int width = jm.Get<int>("window.width", kClientWidth);
			int height = jm.Get<int>("window.height", kClientHeight);
			std::string titleStr = jm.Get<std::string>("window.title", "TYEngine");
			std::wstring title(titleStr.begin(), titleStr.end());

			wc_.lpfnWndProc = WindowProc;

			wc_.lpszClassName = L"TYengineWindowClass";

			wc_.hInstance = GetModuleHandle(nullptr);

			wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

			RegisterClass(&wc_);

			RECT wrc = { 0, 0, width, height };

			AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

			hwnd_ = CreateWindow(
				wc_.lpszClassName,
				title.c_str(),
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				wrc.right - wrc.left,
				wrc.bottom - wrc.top,
				nullptr,
				nullptr,
				wc_.hInstance,
				nullptr);

			// ウィンドウを表示状態にする
			ShowWindow(hwnd_, SW_SHOW);

			// システムタイマーの分解能を上げる（マルチメディアタイマーなどへの影響）
			timeBeginPeriod(1);
		}

		bool WindowsApp::ProcessMessage()
		{
			// メッセージループ
			MSG msg{};
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					return true; // WM_QUIT を検出した時点で true を返す
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return false; // 終了メッセージが無ければ false のまま
		}

		void WindowsApp::Finalize()
		{
			CloseWindow(hwnd_);
			CoUninitialize();
		}

	} // namespace Core
} // namespace TYEngine
