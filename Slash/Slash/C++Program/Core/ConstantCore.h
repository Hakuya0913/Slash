#pragma once
#include<Windows.h>

namespace ConstWindow
{

	//ウィンドウクラスネームとウィンドウネーム
	constexpr LPCWSTR ClassName = L"DX12_Class";
	constexpr LPCWSTR WindowName = ClassName;

	//スクリーンサイズのデフォルト設定
	constexpr UINT ScreenW = 960;
	constexpr UINT ScreenH = 540;

}

namespace ConstD3D
{

	//マルチバッファリング数とフレームリソース数
	constexpr UINT BufferringCount = 2;
	constexpr UINT FrameResourceCount = BufferringCount;

	//解像度
	constexpr UINT ResolutionW = ConstWindow::ScreenW;
	constexpr UINT ResolutionH = ConstWindow::ScreenH;

	//リフレッシュレート
	constexpr UINT RefreshNumrator = 60;
	constexpr UINT RefreshDenomirator = 1;

	//スクリーンクリアカラー
	constexpr float ClearColor[4] = { 0.0f, 1.0f, 6.f,1.0f };

}