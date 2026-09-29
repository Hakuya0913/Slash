#include"Window.h"
#include<vector>
#include"ConstantCore.h"
#include"../Input/InputManager.h"

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	InputManager::GetInstance().GetKeyMouseInput().RegisterRID(hwnd);
	InputManager::GetInstance().GetKeyMouseInput().SetLParam(lParam);

	switch (message) {
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;

		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);

}

void Window::Init() {

	hInstance = GetModuleHandle(nullptr);

	if (hInstance == nullptr) {

		return;

	}

	//ウィンドウ設定
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = ConstWindow::ClassName;
	wc.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	//ウィンドウクラス登録
	RegisterClassEx(&wc);

	//ウィンドウサイズ設定
	RECT rect{};
	rect.right = static_cast<LONG>(ConstWindow::ScreenW);
	rect.bottom = static_cast<LONG>(ConstWindow::ScreenH);

	//ウィンドウサイズを調整
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rect, style, FALSE);

	//ウィンドウを生成
	hwnd = CreateWindowEx(
		0,
		ConstWindow::ClassName,
		ConstWindow::WindowName,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	//ウィンドウを表示
	ShowWindow(hwnd, SW_SHOWNORMAL);

	//ウィンドウにフォーカス
	SetFocus(hwnd);

}