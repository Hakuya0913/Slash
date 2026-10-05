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
		Vector3 cameraPos = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 cameraLookAt = Vector3(0.0f, cameraPos.y + 0.0f, 0.0f);
		camera.SetPosition(cameraPos);
		camera.SetLookAt(cameraLookAt);
		camera.SetPerspective(
			DirectX::XMConvertToRadians(60.0f),
			static_cast<float>(ConstWindow::ScreenW) / static_cast<float>(ConstWindow::ScreenH),
			0.1f, 
			10000.0f
		);

		//モデルの読み込み
		isCorrect = model.Load("Model/effole/effole.fbx");

		if (isCorrect == false) {
			return;
		}

		//モデルは原点に表示するので、TransformComponentの位置は変更しない
		transform.SetRotation(Vector3(0.0f, 0.0f, 0.0f));
		transform.SetScale(Vector3(0.1f, 0.1f, 0.1f));

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

			//camera.Update();
			camera.Update();
			modelRenderer.Render(model, transform);

			gfxDevice.EndFrame();

		}

	}

}
