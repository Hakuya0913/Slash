#pragma once
#include<Windows.h>
#include<Xinput.h>
#include<array>

namespace KeyMouseConst{

	// キーの数
	constexpr int KeyCount		 = 256;

	//取得したいRawInputデバイス数
	//今回は「キーボード＋マウス」の2つ
	constexpr UINT RawInputDeviceCount = 2;
	
	//マウスボタンの割付
	constexpr UINT MouseButtonNum = 5;
	constexpr UINT MouseButtonL	  = 0;
	constexpr UINT MouseButtonR   = 1;
	constexpr UINT MouseButtonM   = 2;
	constexpr UINT MouseButtonX1  = 3;
	constexpr UINT MouseButtonX2  = 4;

}

namespace PadConst {

	//入力の最大値
	constexpr float StickMax	= 32767.0f;
	constexpr UINT TriggerMax	= 255;

	//デッドゾーン
	//「これ未満」ならデッドゾーン圏内として扱う
	constexpr float Deadzone = 0.1f;

	//アナログ入力正規化後の閾値
	//「それ以上」ならその強さとして扱う
	constexpr float ThresholdLow	= 0.1f;
	constexpr float ThresholdMiddle = 0.4f;
	constexpr float ThresholdHigh	= 0.7f;

}

//使用する入力の名前
//連番前提で動作する
enum class PadInputDigital {

	Up,
	Down,
	Left,
	Right,

	A,
	B,
	X,
	Y,

	LB,
	RB,

	LThumb,
	RThumb,

	Start,
	View,

	//要素数
	Count

};

//使用するアナログ入力の名前
//連番前提で動作する
enum class PadInputAnalog1D {

	LT, 
	RT,

	//要素数
	Count

};

enum class PadInputAnalog2D {
	
	LStick,
	RStick,
	
	//要素数
	Count

};

enum class MouseInputAnalog1D {

	Wheel,

	//要素数
	Count

};

enum class MouseInputAnalog2D {
	
	Position,
	Delta,
	
	//要素数
	Count

};

//入力の有無(変化時と継続)
enum class InputState {

	Trigger,	//押下の瞬間
	Hold,		//押下継続
	Release,	//離上の瞬間
	None		//入力なし

};

//アナログ入力の強さ定義(4種)
enum class AnalogStrength {

	Zero,		//入力なし
	Low,		//弱
	Middle,		//中
	High		//強

};

//ゲームで使用するコマンド一覧
//ゲーム本編操作中
enum class GamePlayCommandDigital {

	Jump,
	Dash,
	Attack,
	Guard,
	LockOn,
	MenuOpen,

	//要素数
	Count

};

enum class GamePlayCommandAnalog {

	Move,
	Guard,
	CameraMove,

	//要素数
	Count

};

//メニュー操作中
enum class MenuCommandDigital {

	Up,
	Down,
	Left,
	Right,
	PgUp,
	PgDn,

	Confirm,
	Cancel,

	//要素数
	Count

};

enum class MenuCommandAnalog {

	CursorMove,
	CursorPos,

	//要素数
	Count

};

//入力の解釈状態
enum class InputContext {

	GamePlay,
	Menu

};