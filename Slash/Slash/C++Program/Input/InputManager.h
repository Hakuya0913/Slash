#pragma once

/*

各インプットクラスをまとめる

*/

#include"KeyMouseInput.h"
#include"XInput.h"

class InputManager {
public:

	inline static InputManager& GetInstance() {
		static InputManager instance;
		return instance;
	}

	void Update();

	KeyMouseInput&	GetKeyMouseInput() { return keyMouseInput; }
	XInput&			GetXInput() { return xInput; }

private:

	InputManager() = default;

	KeyMouseInput	keyMouseInput;
	XInput			xInput;

};