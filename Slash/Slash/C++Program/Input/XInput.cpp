#include"XInput.h"
#include<algorithm>

#pragma comment(lib, "XInput.lib")

XInput::XInput(DWORD index)
	: padIndex(index), isConnectCurrent(false) 
{

	InitState();

}

//状態の全初期化
void XInput::InitState()
{

	ZeroMemory(&xInputState, sizeof(_XINPUT_STATE));

	buttonStateCurrent.fill(InputState::None);
	buttonStatePrev.fill(	InputState::None);

	analogValue1DCurrent.fill(	0.0f);
	analogValue1DPrev.fill(		0.0f);
	analogValue1DDelta.fill(	0.0f);

	analogValue2DCurrent.fill(	DirectX::SimpleMath::Vector2::Zero);
	analogValue2DPrev.fill(		DirectX::SimpleMath::Vector2::Zero);
	analogValue2DDelta.fill(	DirectX::SimpleMath::Vector2::Zero);

}

void XInput::Update() 
{

	DWORD result;

	ZeroMemory(&xInputState, sizeof(XINPUT_STATE));
	result = XInputGetState(padIndex, &xInputState);

	isConnectPrev = isConnectCurrent;
	isConnectCurrent = (result == ERROR_SUCCESS);

	//接続中なら状態更新
	if (isConnectCurrent) 
	{

		UpdateDegitalInput();
		UpdateAnalogInput();
		
	}

	//接続が切れた瞬間に状態初期化
	if (isConnectPrev		== true &&
		isConnectCurrent	== false) {

		InitState();

	}

}

void XInput::UpdateDegitalInput()
{

	//過去フレームの状態保持
	buttonStatePrev = buttonStateCurrent;

	auto button = xInputState.Gamepad.wButtons;

	for (int i = 0; i < static_cast<int>(PadInputDigital::Count); ++i)
	{

		bool isPress = (button & ConvertWORD(static_cast<PadInputDigital>(i))) != 0;

		if (isPress) 
		{

			if (buttonStatePrev[i] == InputState::Release ||
				buttonStatePrev[i] == InputState::None) 
			{

				buttonStateCurrent[i] = InputState::Trigger;
				continue;

			}

			if (buttonStatePrev[i] == InputState::Trigger) 
			{

				buttonStateCurrent[i] = InputState::Hold;
				continue;

			}

		}
		else 
		{

			if (buttonStatePrev[i] == InputState::Trigger ||
				buttonStatePrev[i] == InputState::Hold) 
			{

				buttonStateCurrent[i] = InputState::Release;
				continue;

			}

			if (buttonStatePrev[i] == InputState::Release)
			{

				buttonStateCurrent[i] = InputState::None;
				continue;

			}

		}

	}

}

void XInput::UpdateAnalogInput()
{

	//過去フレームの状態保持
	analogValue1DPrev = analogValue1DCurrent;
	analogValue2DPrev = analogValue2DCurrent;

	//1Dアナログ入力の正規化値を更新
	for (int i = 0; i < static_cast<int>(PadInputAnalog1D::Count); ++i)
	{

		//値の更新
		analogValue1DCurrent[i] = ConvertAnalogValue(static_cast<PadInputAnalog1D>(i));

		//デッドゾーン適用
		analogValue1DCurrent[i] = ApplyDeadzone(analogValue1DCurrent[i]);

		//デルタ値の更新
		analogValue1DDelta[i]	= analogValue1DCurrent[i] - analogValue1DPrev[i];
	}

	//2Dアナログ入力の正規化値を更新
	for (int i = 0; i < static_cast<int>(PadInputAnalog2D::Count); ++i)
	{

		//値の更新
		analogValue2DCurrent[i] = ConvertAnalogValue(static_cast<PadInputAnalog2D>(i));

		//デッドゾーン適用
		analogValue2DCurrent[i] = ApplyDeadzoneRadial(analogValue2DCurrent[i]);

		//デルタ値の更新
		analogValue2DDelta[i] = analogValue2DCurrent[i] - analogValue2DPrev[i];

	}

}

//デジタル入力の状態を返す
InputState XInput::GetDigitalState(PadInputDigital padInput) const 
{

	//未接続　or Countが渡されたら 入力なし
	if (!isConnectCurrent)					return InputState::None;
	if (padInput == PadInputDigital::Count) return InputState::None;

	return buttonStateCurrent[static_cast<int>(padInput)];

}

//1Dアナログ入力の強さを返す
AnalogStrength XInput::GetAnalogStrength(PadInputAnalog1D padInput) const
{

	//未接続 or Countが渡されたら 強さ0
	if (!isConnectCurrent)						return AnalogStrength::Zero;
	if (padInput == PadInputAnalog1D::Count)	return AnalogStrength::Zero;

	int index = static_cast<int>(padInput);

	if (analogValue1DCurrent[index] > PadConst::ThresholdHigh)		return AnalogStrength::High;
	if (analogValue1DCurrent[index] > PadConst::ThresholdMiddle)	return AnalogStrength::Middle;
	if (analogValue1DCurrent[index] > PadConst::ThresholdLow)		return AnalogStrength::Low;

	return AnalogStrength::Zero;
}

//1Dアナログ入力の正規化値を返す
float XInput::GetAnalogValue(PadInputAnalog1D padInput) const
{

	//未接続 or Countが渡されたら 入力0
	if (!isConnectCurrent)					 return 0.0f;
	if (padInput == PadInputAnalog1D::Count) return 0.0f;

	int index = static_cast<int>(padInput);

	return analogValue1DCurrent[index];

}

//1Dアナログ入力のデルタ値を返す
float XInput::GetAnalogDelta(PadInputAnalog1D padInput) const 
{
	
	//未接続 or Countが渡されたら 入力0
	if (!isConnectCurrent)					 return 0.0f;
	if (padInput == PadInputAnalog1D::Count) return 0.0f;

	int index = static_cast<int>(padInput);

	return analogValue1DDelta[index];

}

//2Dアナログ入力の強さを返す
AnalogStrength XInput::GetAnalogStrength(PadInputAnalog2D padInput) const
{
	//未接続状態　or Countが渡されたら 強さ0
	if (!isConnectCurrent)						return AnalogStrength::Zero;
	if (padInput == PadInputAnalog2D::Count)	return AnalogStrength::Zero;

	int index	 = static_cast<int>(padInput);
	float length = analogValue2DCurrent[index].Length();

	if (length > PadConst::ThresholdHigh)	return AnalogStrength::High;
	if (length > PadConst::ThresholdMiddle)	return AnalogStrength::Middle;
	if (length > PadConst::ThresholdLow)	return AnalogStrength::Low;
	
	return	AnalogStrength::Zero;

}

//2Dアナログ入力の正規化値を返す
DirectX::SimpleMath::Vector2 XInput::GetAnalogValue(PadInputAnalog2D padInput) const
{

	//未接続なら入力0
	if (!isConnectCurrent)					 return DirectX::SimpleMath::Vector2::Zero;
	if (padInput == PadInputAnalog2D::Count) return DirectX::SimpleMath::Vector2::Zero;

	int index = static_cast<int>(padInput);

	return analogValue2DCurrent[index];
}

//2Dアナログ入力のデルタ値を返す
DirectX::SimpleMath::Vector2 XInput::GetAnalogDelta(PadInputAnalog2D padInput) const
{

	//Countが渡されたら 入力0
	if (padInput == PadInputAnalog2D::Count) return DirectX::SimpleMath::Vector2::Zero;

	int index = static_cast<int>(padInput);

	return analogValue2DDelta[index];

}

//PadInputを意味を持つビット列で返す
WORD XInput::ConvertWORD(PadInputDigital padInput) const
{

	switch (padInput)
	{

	case PadInputDigital::Up:		return XINPUT_GAMEPAD_DPAD_UP;
	case PadInputDigital::Down:		return XINPUT_GAMEPAD_DPAD_DOWN;
	case PadInputDigital::Left:		return XINPUT_GAMEPAD_DPAD_LEFT;
	case PadInputDigital::Right:	return XINPUT_GAMEPAD_DPAD_RIGHT;

	case PadInputDigital::A:		return XINPUT_GAMEPAD_A;
	case PadInputDigital::B:		return XINPUT_GAMEPAD_B;
	case PadInputDigital::X:		return XINPUT_GAMEPAD_X;
	case PadInputDigital::Y:		return XINPUT_GAMEPAD_Y;

	case PadInputDigital::LB:		return XINPUT_GAMEPAD_LEFT_SHOULDER;
	case PadInputDigital::RB:		return XINPUT_GAMEPAD_RIGHT_SHOULDER;

	case PadInputDigital::LThumb:	return XINPUT_GAMEPAD_LEFT_THUMB;
	case PadInputDigital::RThumb:	return XINPUT_GAMEPAD_RIGHT_THUMB;

	case PadInputDigital::Start:	return XINPUT_GAMEPAD_START;
	case PadInputDigital::View:		return XINPUT_GAMEPAD_BACK;

	default:						return 0;

	}

}

//1Dアナログ入力の正規化値を返す
float XInput::ConvertAnalogValue(PadInputAnalog1D padInput) const 
{

	auto gamepad = xInputState.Gamepad;

	float result;

	switch (padInput) 
	{

	case PadInputAnalog1D::LT:		result = static_cast<float>(gamepad.bLeftTrigger)	/ PadConst::TriggerMax; break;
	case PadInputAnalog1D::RT:		result = static_cast<float>(gamepad.bRightTrigger)	/ PadConst::TriggerMax;	break;

	default:						result = 0.0f;

	}

	//-1～1の範囲に正規化結果を収める
	result = std::clamp(result, -1.0f, 1.0f);

	return result;

}

//2Dアナログ入力の正規化値を返す
DirectX::SimpleMath::Vector2 XInput::ConvertAnalogValue(PadInputAnalog2D padInput) const
{

	auto gamepad = xInputState.Gamepad;

	DirectX::SimpleMath::Vector2 result;

	switch (padInput)
	{

	case PadInputAnalog2D::LStick:	result = DirectX::SimpleMath::Vector2(static_cast<float>(gamepad.sThumbLX), static_cast<float>(gamepad.sThumbLY));	break;
	case PadInputAnalog2D::RStick:	result = DirectX::SimpleMath::Vector2(static_cast<float>(gamepad.sThumbRX), static_cast<float>(gamepad.sThumbRY));	break;

	default:						result = DirectX::SimpleMath::Vector2::Zero;

	}

	//-1～1の範囲に正規化
	result.x /= PadConst::StickMax;
	result.y /= PadConst::StickMax;

	//-1～1の範囲にクリップ
	result.x = std::clamp(result.x, -1.0f, 1.0f);
	result.y = std::clamp(result.y, -1.0f, 1.0f);

	return result;

}

//2Dアナログ入力の円形デッドゾーン適用
DirectX::SimpleMath::Vector2 XInput::ApplyDeadzoneRadial(DirectX::SimpleMath::Vector2 vector) const 
{

	float length = vector.Length();

	auto deadzone = PadConst::Deadzone;

	if (length < deadzone) 
	{

		return DirectX::SimpleMath::Vector2::Zero;

	}

	float scale = (length - deadzone) / (1.0f - deadzone);

	return vector / length * scale;

}

float XInput::ApplyDeadzone(float value) const 
{
	
	float absValue = std::abs(value);
	auto deadzone = PadConst::Deadzone;

	if (absValue < deadzone) 
	{

		return 0.0f;

	}

	return value;

}