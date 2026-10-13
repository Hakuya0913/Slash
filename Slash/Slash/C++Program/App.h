#pragma once

/*

ウィンドウの生成等を管理するアプリケーションクラス

*/

#include"Core/Window.h"
#include"Core/GraphicsDevice.h"

//試験用インスタンス等のヘッダファイル
#include"Model/ModelComponent.h"
#include"Component/TransformComponent.h"
#include"Renderer/ModelRenderer.h"
#include"Component/Camera.h"
#include"Model/ModelResource.h"

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

	//試験用構造体
	struct CharaObject
	{

		TransformComponent transform;
		ModelComponent model;
		ModelResource resource;

	};

	CharaObject effole;
	CharaObject radDoll;
	Camera camera;
	ModelRenderer modelRenderer;

};

