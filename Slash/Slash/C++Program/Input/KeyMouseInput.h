#pragma once

/*

キーボード・マウスの入力を管理
InterfaceInputを継承した
RawInputアダプタークラス

*/

#include<Windows.h>
#include<DirectXMath.h>
#include<SimpleMath.h>
#include<vector>
#include"ConstantInput.h"
#include"../Math/StructureMath.h"

class KeyMouseInput
{
public:

	KeyMouseInput();

	void Update();

	//ゲッター
	InputState GetKeyState(USHORT vk)			const { return keyStateCurrent[vk]; }
	InputState GetMouseButtonState(UINT number) const { return mouseButtonStateCurrent[number]; }

	DirectX::SimpleMath::Vector2 GetMousePos()	 const { return mousePosCurrent; }
	Vector2_LONG				 GetMouseDelta() const { return mouseDeltaCurrent; }
	SHORT						 GetWheelDelta() const { return wheelDeltaCurrent; }


	//キーマウ入力受け取り系
	void RegisterRID(HWND hwnd);
	void SetLParam(LPARAM lParam);

private:

	//生入力の状態更新
	void SetKeyboard(USHORT vk, bool isPress);
	void SetMouseButton(USHORT number, bool isPress);
	void SetMouseMove(LONG dx, LONG dy);
	void SetMouseWheel(SHORT delta);

	//InputState更新
	void UpdateKeyboard();
	void UpdateMouseButton();

	//キーボード入力
	std::vector<bool>		isKeyPress;	//生値で押されているか
	std::vector<InputState> keyStateCurrent;
	std::vector<InputState> keyStatePrev;

	//マウスのボタン入力
	std::vector<bool>		isMouseButtonPress;	//生値で押されているか
	std::vector<InputState> mouseButtonStateCurrent;
	std::vector<InputState> mouseButtonStatePrev;

	//マウス移動
	Vector2_LONG mouseDeltaCurrent;

	//マウス位置
	DirectX::SimpleMath::Vector2 mousePosCurrent;
	DirectX::SimpleMath::Vector2 mousePosPrev;
	
	//マウスホイール
	SHORT wheelDeltaCurrent;


};