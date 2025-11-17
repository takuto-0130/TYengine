#include "Input.h"
#include <stdexcept>
#include <assert.h>

constexpr float kBaseWidth = 1280.0f;
constexpr float kBaseHeight = 720.0f;

// 初期化処理
void Input::Initialize(const HWND& hwnd) 
{
    HRESULT hr;
    clientHwnd_ = hwnd;
    hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dInput_, nullptr);
    assert(SUCCEEDED(hr));

    hr = dInput_->CreateDevice(GUID_SysKeyboard, &devKeyboard_, nullptr);
    assert(SUCCEEDED(hr));

    hr = dInput_->CreateDevice(GUID_SysMouse, &devMouse_, nullptr);
    assert(SUCCEEDED(hr));

    devKeyboard_->SetDataFormat(&c_dfDIKeyboard);
    devKeyboard_->SetCooperativeLevel(clientHwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    devKeyboard_->Acquire();

    devMouse_->SetDataFormat(&c_dfDIMouse2);
    devMouse_->SetCooperativeLevel(clientHwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    devMouse_->Acquire();

    dInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
}

void Input::Finalize()
{
    // デバイスの取得解除
    if (devKeyboard_)
    {
        devKeyboard_->Unacquire();
    }
    if (devMouse_)
    {
        devMouse_->Unacquire();
    }
    for (auto& joystick : devJoysticks_)
    {
        if (joystick.device_)
        {
            joystick.device_->Unacquire();
        }
    }

    // COM オブジェクト解放（ここで DirectInput のリソースを手放す）
    devJoysticks_.clear();
    devMouse_.Reset();
    devKeyboard_.Reset();
    dInput_.Reset();
}

void Input::Update()
{
    if (!devKeyboard_ || !devMouse_)
    {
        return; // 未初期化 / 取得失敗時の安全対策
    }

    // 前フレーム保存
    keyPre_ = key_;
    mousePre_ = mouse_;

    // キーボード
    HRESULT hr = devKeyboard_->GetDeviceState(sizeof(key_), key_.data());
    if (FAILED(hr))
    {
        devKeyboard_->Acquire();  // 再取得
        devKeyboard_->GetDeviceState(sizeof(key_), key_.data());
    }

    // マウス
    hr = devMouse_->GetDeviceState(sizeof(DIMOUSESTATE2), &mouse_);
    if (FAILED(hr))
    {
        devMouse_->Acquire();     // 再取得
        devMouse_->GetDeviceState(sizeof(DIMOUSESTATE2), &mouse_);
    }

    // ジョイスティック
    for (auto& joystick : devJoysticks_)
    {
        joystick.statePre_ = joystick.state_;

        if (joystick.type_ == PadType::XInput)
        {
            XInputGetState(0, &joystick.state_.xInput_);
        }
        else if (joystick.device_)
        {
            joystick.device_->Poll();
            joystick.device_->GetDeviceState(sizeof(DIJOYSTATE2), &joystick.state_.directInput_);
        }
    }
}


bool Input::PushKey(BYTE keyNumber) const 
{
    return key_[keyNumber] & 0x80;
}

bool Input::TriggerKey(BYTE keyNumber) const 
{
    return (key_[keyNumber] & 0x80) && !(keyPre_[keyNumber] & 0x80);
}

const DIMOUSESTATE2& Input::GetAllMouse() const 
{
    return mouse_;
}

bool Input::IsPressMouse(int32_t buttonNumber) const
{
    return mouse_.rgbButtons[buttonNumber] & 0x80;
}

bool Input::IsTriggerMouse(int32_t buttonNumber) const
{
    return (mouse_.rgbButtons[buttonNumber] & 0x80) && !(mousePre_.rgbButtons[buttonNumber] & 0x80);
}

Input::MouseMove Input::GetMouseMove()
{
    MouseMove move = { mouse_.lX, mouse_.lY, mouse_.lZ };
    return move;
}

int32_t Input::GetWheel() const 
{
    return mouse_.lZ;
}

Vector2 Input::GetMousePosition() 
{
    POINT point;
    if (GetCursorPos(&point)) 
    {
        if (clientHwnd_) 
        {
            ScreenToClient(clientHwnd_, &point);
        }
    }

    RECT rect{};
    if (clientHwnd_) 
    {
        GetClientRect(clientHwnd_, &rect);
    }
    float width = static_cast<float>(rect.right - rect.left);
    float height = static_cast<float>(rect.bottom - rect.top);

    if (width <= 0.0f || height <= 0.0f)
    {
        return mousePosition_;
    }

    mousePosition_.x = (point.x / width) * kBaseWidth;
    mousePosition_.y = (point.y / height) * kBaseHeight;

    return mousePosition_;
}

Vector2 Input::GetMousePositionRelative() const
{
    POINT point;
    if (GetCursorPos(&point)) 
    {
        if (clientHwnd_) 
        {
            ScreenToClient(clientHwnd_, &point);
        }
    }

    RECT rect{};
    GetClientRect(clientHwnd_, &rect);
    float width = static_cast<float>(rect.right - rect.left);
    float height = static_cast<float>(rect.bottom - rect.top);

    if (width <= 0.0f || height <= 0.0f)
    {
        return mousePosition_;
    }

    // 0～1 の相対座標を返す
    return
    {
        point.x / width,
        point.y / height
    };
}

Vector2 Input::GetClientSize() const
{
    RECT rect{};
    GetClientRect(clientHwnd_, &rect);
    float width = static_cast<float>(rect.right - rect.left);
    float height = static_cast<float>(rect.bottom - rect.top);
    return { width, height };
}

bool Input::GetJoystickState(int32_t stickNo, DIJOYSTATE2& out) const 
{
    if (stickNo >= devJoysticks_.size()) return false;
    out = devJoysticks_[stickNo].state_.directInput_;
    return true;
}

bool Input::GetJoystickStatePrevious(int32_t stickNo, DIJOYSTATE2& out) const
{
    if (stickNo >= devJoysticks_.size()) return false;
    out = devJoysticks_[stickNo].statePre_.directInput_;
    return true;
}

bool Input::GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const 
{
    if (stickNo >= devJoysticks_.size()) return false;
    out = devJoysticks_[stickNo].state_.xInput_;
    return true;
}

bool Input::GetJoystickStatePrevious(int32_t stickNo, XINPUT_STATE& out) const
{
    if (stickNo >= devJoysticks_.size()) return false;
    out = devJoysticks_[stickNo].statePre_.xInput_;
    return true;
}

void Input::SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR) 
{
    if (stickNo >= devJoysticks_.size()) return;
    devJoysticks_[stickNo].deadZoneL_ = deadZoneL;
    devJoysticks_[stickNo].deadZoneR_ = deadZoneR;
}

// 接続されているジョイスティックの数を取得
size_t Input::GetNumberOfJoysticks()
{
    return devJoysticks_.size();
}

BOOL CALLBACK Input::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) noexcept 
{
    auto input = static_cast<Input*>(pContext);
    Microsoft::WRL::ComPtr<IDirectInputDevice8> joystick;

    if (FAILED(input->dInput_->CreateDevice(pdidInstance->guidInstance, &joystick, nullptr))) 
    {
        return DIENUM_CONTINUE;
    }

    Joystick newJoystick = {};
    newJoystick.device_ = joystick;
    newJoystick.type_ = PadType::DirectInput;
    input->devJoysticks_.push_back(newJoystick);

    return DIENUM_CONTINUE;
}
