#include "WindowsApp.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
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

			wc.lpfnWndProc = WindowProc;

			wc.lpszClassName = L"TYengineWindowClass";

			wc.hInstance = GetModuleHandle(nullptr);

			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

			RegisterClass(&wc);

			RECT wrc = { 0, 0, kClientWidth, kClientHeight };

			AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

			hwnd = CreateWindow(
				wc.lpszClassName,
				L"TYengine",
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				wrc.right - wrc.left,
				wrc.bottom - wrc.top,
				nullptr,
				nullptr,
				wc.hInstance,
				nullptr);

			// ウィンドウを表示状態にする
			ShowWindow(hwnd, SW_SHOW);

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
			CloseWindow(hwnd);
			CoUninitialize();
		}

	} // namespace Core
} // namespace TYEngine
