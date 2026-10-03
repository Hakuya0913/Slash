#pragma once

/*

ウィンドウの生成等を管理するアプリケーションクラス

*/

#include"Core/Window.h"
#include"Core/GraphicsDevice.h"

//試験用インスタンス等のヘッダファイル
#include"Component/ModelComponent.h"
#include"Component/TransformComponent.h"
#include"Renderer/ModelRenderer.h"
#include"Camera/Camera.h"

class App
{
public:

	App();
	~App() = default;

	void Init();
	void Update();

private:

	Window window;

	//試験用インスタンス等
	ModelComponent model;
	TransformComponent transform;
	Camera camera;
	ModelRenderer modelRenderer;

};