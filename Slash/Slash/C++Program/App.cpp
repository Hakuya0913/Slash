#include"App.h"
#include"Input/InputManager.h"

#ifdef _DEUG

#include<sstream>

#endif // _DEUG

App::App()
{

	

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
		const Vector3 cameraPos = Vector3(0.0f, 50.0f, -300.0f);
		const Vector3 cameraLookAt = Vector3(0.0f, 40.0f, 0.0f);
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
		//transform.SetRotation(Vector3(0.0f, 50.0f, 0.0f));
		//transform.SetScale(Vector3(0.1f, 0.1f, 0.1f));

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

			const float power = 1.0f;
			if (input.GetKeyMouseInput().GetKeyState('D') == InputState::Hold)
			{

				//transform.AddPosition(DirectX::SimpleMath::Vector3(power, 0.0f, 0.0f));

				camera.GetTransform().AddPosition(DirectX::SimpleMath::Vector3(power, 0.0f, 0.0f));
				camera.SetLookAt(camera.GetLookAt() + DirectX::SimpleMath::Vector3(power, 0.0f, 0.0f));

			}
			if (input.GetKeyMouseInput().GetKeyState('A') == InputState::Hold)
			{

				//transform.AddPosition(DirectX::SimpleMath::Vector3(-power, 0.0f, 0.0f));

				camera.GetTransform().AddPosition(DirectX::SimpleMath::Vector3(-power, 0.0f, 0.0f));
				camera.SetLookAt(camera.GetLookAt() + DirectX::SimpleMath::Vector3(-power, 0.0f, 0.0f));

			}
			if (input.GetKeyMouseInput().GetKeyState('W') == InputState::Hold)
			{

				//transform.AddPosition(DirectX::SimpleMath::Vector3(0.0f, 0.0f, power));

				camera.GetTransform().AddPosition(DirectX::SimpleMath::Vector3(0.0f, 0.0f, power));
				camera.SetLookAt(camera.GetLookAt() + DirectX::SimpleMath::Vector3(0.0f, 0.0f, power));

			}
			if (input.GetKeyMouseInput().GetKeyState('S') == InputState::Hold)
			{

				//transform.AddPosition(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -power));

				camera.GetTransform().AddPosition(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -power));
				camera.SetLookAt(camera.GetLookAt() + DirectX::SimpleMath::Vector3(0.0f, 0.0f, -power));

			}
			if (input.GetKeyMouseInput().GetKeyState(VK_SPACE) == InputState::Hold)
			{

				//transform.AddPosition(DirectX::SimpleMath::Vector3(0.0f, power, 0.0f));

				camera.GetTransform().AddPosition({ 0.0f,power,0.0f });
				camera.SetLookAt(camera.GetLookAt() + DirectX::SimpleMath::Vector3(0.0f, power, 0.0f));

			}
			if (input.GetKeyMouseInput().GetKeyState(VK_SHIFT) == InputState::Hold)
			{

				//transform.AddPosition(DirectX::SimpleMath::Vector3(0.0f, -power, 0.0f));

				camera.GetTransform().AddPosition({ 0.0f,-power,0.0f });
				camera.SetLookAt(camera.GetLookAt() + DirectX::SimpleMath::Vector3(0.0f, -power, 0.0f));

			}

			//XMMatrixIndentityを使用している際の軸確認用
			//transform.AddPosition(DirectX::SimpleMath::Vector3(-0.1f, 0.0f, 0.0f));

			//camera.Update();
			camera.Update();
			modelRenderer.Render(model, transform);

			gfxDevice.EndFrame();

		}

	}

}
