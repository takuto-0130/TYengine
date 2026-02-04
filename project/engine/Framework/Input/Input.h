#pragma once
#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>
#include <XInput.h>
#include <Windows.h>
#include <array>
#include <vector>
#include <wrl.h>
#include <mutex>

#include "struct.h"
#include "SingletonObject.h"

namespace TYEngine {
namespace Framework {

// using namespace Utility;

/// <summary>
/// キーボード、マウス、ジョイスティックの入力管理クラス（シングルトン）。
/// DirectInput8 および XInput を使用して入力状態を取得する。
/// </summary>
class Input :
	public TYEngine::Utility::SingletonObject<Input>
{
	friend class TYEngine::Utility::SingletonObject<Input>;
	friend struct std::default_delete<Input>;

private:
	// 外部からの new/delete を禁止
	Input() = default;
	~Input() = default;

public: // メンバ関数
	/// <summary>
	/// 終了処理。
	/// DirectInputデバイスの解放を行う。
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化処理。
	/// DirectInput8オブジェクトの生成、キーボード・マウスデバイスの初期化を行う。
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル。</param>
	void Initialize(const HWND& hwnd);

	/// <summary>
	/// 毎フレームの更新処理。
	/// キーボード、マウス、ジョイスティックの状態を取得・保存する。
	/// </summary>
	void Update();

	/// <summary>
	/// 指定したキーが押され続けているか判定する。
	/// </summary>
	/// <param name="keyNumber">キー番号（DIK_*）。</param>
	/// <returns>押されているなら true。</returns>
	bool PushKey(BYTE keyNumber) const;

	/// <summary>
	/// 指定したキーがこのフレームで押されたか（トリガー）判定する。
	/// </summary>
	/// <param name="keyNumber">キー番号（DIK_*）。</param>
	/// <returns>トリガーされたなら true。</returns>
	bool TriggerKey(BYTE keyNumber) const;

	/// <summary>
	/// 全キーの入力状態を取得する。
	/// </summary>
	/// <returns>256キーの状態配列。</returns>
	const std::array<BYTE, 256>& GetAllKey() const { return key_; }

	/// <summary>
	/// 全マウスボタンおよび移動量の状態を取得する。
	/// </summary>
	/// <returns>マウス状態構造体。</returns>
	const DIMOUSESTATE2& GetAllMouse() const;

	/// <summary>
	/// 指定したマウスボタンが押され続けているか判定する。
	/// </summary>
	/// <param name="mouseNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張)。</param>
	/// <returns>押されているなら true。</returns>
	bool IsPressMouse(int32_t mouseNumber) const;

	/// <summary>
	/// 指定したマウスボタンがこのフレームで押されたか（トリガー）判定する。
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張)。</param>
	/// <returns>トリガーされたなら true。</returns>
	bool IsTriggerMouse(int32_t buttonNumber) const;

	/// <summary>
	/// マウスの移動量を取得する。
	/// </summary>
	/// <returns>X, Y, Z（ホイール）の移動量。</returns>
	TYEngine::Utility::Vector3 GetMouseMove();

	/// <summary>
	/// マウスホイールのスクロール量を取得する。
	/// </summary>
	/// <returns>スクロール量。</returns>
	int32_t GetWheel() const;

	/// <summary>
	/// クライアント領域基準のマウス座標を取得する。
	/// </summary>
	/// <returns>ピクセル単位のマウス座標。</returns>
	TYEngine::Utility::Vector2 GetMousePosition();

	/// <summary>
	/// クライアント領域内の正規化されたマウス位置を取得する。
	/// </summary>
	/// <returns>0.0～1.0 の範囲の相対座標。</returns>
	TYEngine::Utility::Vector2 GetMousePositionRelative() const;

	/// <summary>
	/// クライアント領域のサイズを取得する。
	/// </summary>
	/// <returns>幅と高さ。</returns>
	TYEngine::Utility::Vector2 GetClientSize() const;

	/// <summary>
	/// 現在のジョイスティック状態を取得する（DirectInput/XInput対応）。
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号。</param>
	/// <param name="out">状態格納用構造体（DIJOYSTATE2）。</param>
	/// <returns>取得成功なら true。</returns>
	bool GetJoystickState(int32_t stickNo, DIJOYSTATE2& out) const;

	/// <summary>
	/// 前フレームのジョイスティック状態を取得する。
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号。</param>
	/// <param name="out">状態格納用構造体（DIJOYSTATE2）。</param>
	/// <returns>取得成功なら true。</returns>
	bool GetJoystickStatePrevious(int32_t stickNo, DIJOYSTATE2& out) const;

	/// <summary>
	/// 現在のジョイスティック状態を取得する（XInput用）。
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号。</param>
	/// <param name="out">状態格納用構造体（XINPUT_STATE）。</param>
	/// <returns>取得成功なら true。</returns>
	bool GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const;

	/// <summary>
	/// 前フレームのジョイスティック状態を取得する（XInput用）。
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号。</param>
	/// <param name="out">状態格納用構造体（XINPUT_STATE）。</param>
	/// <returns>取得成功なら true。</returns>
	bool GetJoystickStatePrevious(int32_t stickNo, XINPUT_STATE& out) const;

	/// <summary>
	/// ジョイスティックのデッドゾーンを設定する。
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号。</param>
	/// <param name="deadZoneL">左スティックのデッドゾーン（0~32768）。</param>
	/// <param name="deadZoneR">右スティックのデッドゾーン（0~32768）。</param>
	void SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR);

	/// <summary>
	/// 接続されているジョイスティックの数を取得する。
	/// </summary>
	/// <returns>接続数。</returns>
	size_t GetNumberOfJoysticks();

private:
	/// <summary>
	/// ジョイスティック列挙コールバック関数。
	/// 接続されたデバイスごとに呼び出される。
	/// </summary>
	static BOOL CALLBACK
		EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) noexcept;

public:
	/// <summary>パッドの種類。</summary>
	enum class PadType
	{
		DirectInput, ///< DirectInput形式。
		XInput,      ///< XInput形式。
	};

	/// <summary>入力状態共用体（DirectInput / XInput）。</summary>
	union State
	{
		XINPUT_STATE xInput_;       ///< XInput用状態。
		DIJOYSTATE2 directInput_;   ///< DirectInput用状態。
	};

	struct Joystick
	{
		/// <summary>DirectInput デバイスインターフェース。</summary>
		Microsoft::WRL::ComPtr<IDirectInputDevice8> device_;
		
		/// <summary>左スティックのデッドゾーンしきい値。</summary>
		int32_t deadZoneL_;
		
		/// <summary>右スティックのデッドゾーンしきい値。</summary>
		int32_t deadZoneR_;
		
		/// <summary>パッドの種類（DirectInput / XInput）。</summary>
		PadType type_;
		
		/// <summary>現在の入力状態。</summary>
		State state_;
		
		/// <summary>前フレームの入力状態。</summary>
		State statePre_;
	};
private: // メンバ変数
	/// <summary>DirectInput8 インターフェース。</summary>
	Microsoft::WRL::ComPtr<IDirectInput8> dInput_;
	
	/// <summary>キーボードデバイス。</summary>
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devKeyboard_;
	
	/// <summary>マウスデバイス。</summary>
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devMouse_;
	
	/// <summary>接続されているジョイスティック（ゲームパッド）のリスト。</summary>
	std::vector<Joystick> devJoysticks_;
	
	/// <summary>現在の全キー入力状態。</summary>
	std::array<BYTE, 256> key_{};
	
	/// <summary>前フレームの全キー入力状態（トリガー判定用）。</summary>
	std::array<BYTE, 256> keyPre_{};
	
	/// <summary>現在のマウス入力状態。</summary>
	DIMOUSESTATE2 mouse_;
	
	/// <summary>前フレームのマウス入力状態（トリガー判定用）。</summary>
	DIMOUSESTATE2 mousePre_;
	
	/// <summary>入力対象のクライアントウィンドウハンドル。</summary>
	HWND clientHwnd_;
	
	/// <summary>現在のマウスカーソル位置（クライアント座標）。</summary>
	TYEngine::Utility::Vector2 mousePosition_ = {};
};

} // namespace Framework
} // namespace TYEngine
