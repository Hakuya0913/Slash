#include"App.h"
#include"Input/InputManager.h"

App::App()
{

	//特に処理なし

}

void App::Init() {

	window.Init();
	GraphicsDevice::GetInstance().Init(window.GetHWND());

	//試験用インスタンス等の初期化
	{

		bool isCorrect;

		using namespace DirectX::SimpleMath;

		auto& gfxDevice = GraphicsDevice::GetInstance();

		isCorrect = modelRenderer.Init(gfxDevice.GetDevice(),gfxDevice.GetContext(),camera);

		if (isCorrect == false) {
			
			return;

		}

		//カメラの設定
		camera.SetPosition(Vector3(0.0f, 25.0f, -20.0f));
		camera.SetLookAt(Vector3(0.0f, 10.0f, 0.0f));
		camera.SetPerspective(
			DirectX::XMConvertToRadians(60.0f),
			static_cast<float>(gfxDevice.GetResolutionW()) / static_cast<float>(gfxDevice.GetResolutionH()),
			0.1f, 
			1000.0f
		);

		//モデルの読み込み
		isCorrect = model.Load("Model/effole/effole.fbx");

		if (isCorrect == false) {
			return;
		}

		//モデルは原点に表示するので、TransformComponentの位置は変更しない

		//モデルリソースの作成
		isCorrect = modelRenderer.CreateModelResource(model);

		if (isCorrect == false) {
			return;
		}

	}

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

			modelRenderer.Render(model, transform);

			gfxDevice.EndFrame();

		}

	}

}
