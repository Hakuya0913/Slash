#include"App.h"
#include"Input/InputManager.h"

App::App()
{

	//特に処理なし

}

void App::Init() {

	window.Init();
	GraphicsDevice::GetInstance().Init(window.GetHWND());

}

void App::Update() {

	MSG message{};
	auto gfxDevice = GraphicsDevice::GetInstance();

	while (message.message != WM_QUIT) {

		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {

			TranslateMessage(&message);
			DispatchMessage(&message);

		}
		else {

			auto gfxDevice = GraphicsDevice::GetInstance();

			//各オブジェクトの更新処理など
			gfxDevice.BeginFrame();


			auto& input = InputManager::GetInstance();
			input.Update();

			//デバッグ用コード
			{

				

			}

			gfxDevice.EndFrame();

		}

	}

}
