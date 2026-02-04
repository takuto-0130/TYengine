#pragma once
#include <Windows.h>
#include <cstdint>

namespace TYEngine {
namespace Core {

/// <summary>
/// Windows API をラップしてウィンドウ生成・管理を行うクラス。
/// メッセージプロシージャの制御やウィンドウクラスの登録を担当する。
/// </summary>
class WindowsApp
{
public:// 静的メンバ関数
	/// <summary>
	/// ウィンドウプロシージャ。
	/// Windows からのメッセージを受け取り、ImGui やゲーム側の処理へ振り分ける。
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル。</param>
	/// <param name="msg">メッセージ ID。</param>
	/// <param name="wparam">メッセージのパラメータ1。</param>
	/// <param name="lparam">メッセージのパラメータ2。</param>
	/// <returns>メッセージ処理の結果。</returns>
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:// 動的メンバ関数
	/// <summary>
	/// ウィンドウの初期化を行う。
	/// ウィンドウクラスの登録、ウィンドウの生成、表示を行う。
	/// </summary>
	void Initialize();

	/// <summary>
	/// メッセージループを処理する。
	/// </summary>
	/// <returns>終了メッセージ (WM_QUIT) が来たら true、それ以外は false。</returns>
	bool ProcessMessage();

	/// <summary>
	/// 生成されたウィンドウのハンドルを取得する。
	/// </summary>
	/// <returns>HWND ハンドル。</returns>
	HWND GetHwnd() const { return hwnd; }

	/// <summary>
	/// アプリケーションインスタンスハンドルを取得する。
	/// </summary>
	/// <returns>HINSTANCE ハンドル。</returns>
	HINSTANCE GetHInstance() const { return wc.hInstance; }

	/// <summary>
	/// 終了処理。
	/// ウィンドウクラスの登録解除などを行う。
	/// </summary>
	void Finalize();

public:// 定数
	/// <summary>クライアント領域の幅（ピクセル）。</summary>
	static const int32_t kClientWidth = 1280;
	/// <summary>クライアント領域の高さ（ピクセル）。</summary>
	static const int32_t kClientHeight = 720;
	
private:
	/// <summary>管理しているウィンドウハンドル。</summary>
	HWND hwnd = nullptr;
	/// <summary>ウィンドウクラス設定構造体。</summary>
	WNDCLASS wc{};
};

} // namespace Core
} // namespace TYEngine

