#pragma once

/*

Windows標準のXInputを使用した
XInput入力状態管理クラス

*/

#include<Windows.h>
#include<Xinput.h>
#include<array>
#include<DirectXMath.h>
#include<SimpleMath.h>
#include"ConstantInput.h"

class XInput
{
public:

	XInput(DWORD index = 0);

	void Update();

	//Getter
	bool IsConnect() const { return isConnectCurrent; }

	//デジタル入力
	InputState		GetDigitalState(	PadInputDigital padInput) const;

	//1Dアナログ入力
	AnalogStrength	GetAnalogStrength(	PadInputAnalog1D  padInput) const;
	float			GetAnalogValue(		PadInputAnalog1D  padInput) const;
	float			GetAnalogDelta(		PadInputAnalog1D  padInput) const;

	//2Dアナログ入力
	AnalogStrength					GetAnalogStrength(	PadInputAnalog2D  padInput) const;
	DirectX::SimpleMath::Vector2	GetAnalogValue(		PadInputAnalog2D  padInput) const;
	DirectX::SimpleMath::Vector2	GetAnalogDelta(		PadInputAnalog2D  padInput) const;

private:

	//PadInputを意味を持つ数字で返す
	WORD						 ConvertWORD(		PadInputDigital padInput)	const;	//XINPUT_GAMEPAD系のビットフラグに変換
	float						 ConvertAnalogValue(PadInputAnalog1D	padInput)	const;	//アナログ入力の正規化値を返す
	DirectX::SimpleMath::Vector2 ConvertAnalogValue(PadInputAnalog2D	padInput)	const;	//アナログ入力の正規化値を返す

	//全状態初期化
	void InitState();

	//状態の更新
	void UpdateDegitalInput();
	void UpdateAnalogInput();

	//デッドゾーン適用
	DirectX::SimpleMath::Vector2 ApplyDeadzoneRadial(DirectX::SimpleMath::Vector2 vector)	const;	//円形デッドゾーン
	float						 ApplyDeadzone(float value)									const;	//線形デッドゾーン

	DWORD padIndex;

	bool isConnectCurrent;
	bool isConnectPrev;

	//XInputの状態を保存
	XINPUT_STATE xInputState;

	//ボタン入力の状態を保存
	std::array<InputState, static_cast<size_t>(PadInputDigital::Count)> buttonStateCurrent;
	std::array<InputState, static_cast<size_t>(PadInputDigital::Count)> buttonStatePrev;

	//1Dアナログ入力の正規化値を保存
	std::array<float, static_cast<size_t>(PadInputAnalog1D::Count)> analogValue1DCurrent;
	std::array<float, static_cast<size_t>(PadInputAnalog1D::Count)> analogValue1DPrev;
	std::array<float, static_cast<size_t>(PadInputAnalog1D::Count)> analogValue1DDelta;

	//2Dアナログ入力の正規化値を保存
	std::array<DirectX::SimpleMath::Vector2, static_cast<size_t>(PadInputAnalog2D::Count)> analogValue2DCurrent;
	std::array<DirectX::SimpleMath::Vector2, static_cast<size_t>(PadInputAnalog2D::Count)> analogValue2DPrev;
	std::array<DirectX::SimpleMath::Vector2, static_cast<size_t>(PadInputAnalog2D::Count)> analogValue2DDelta;

};
