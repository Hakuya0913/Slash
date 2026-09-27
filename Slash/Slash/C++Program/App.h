#pragma once

/*

ウィンドウの生成等を管理するアプリケーションクラス

*/

#include"Core/Window.h"
#include"Core/GraphicsDevice.h"

class App
{
public:

	App();
	~App() = default;

	void Init();
	void Update();

private:

	Window window;

};