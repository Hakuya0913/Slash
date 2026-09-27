#pragma once

/*

GPU制御クラスのヘッダー
初期化～各フレームでの開始・終了・同期時処理を行う

*/

#include<Windows.h>
#include<d3d11.h>
#include<dxgi.h>

#include"../Utility/ComPtr.h"
#include"ConstantCore.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")

class GraphicsDevice
{
public:

	inline static GraphicsDevice& GetInstance()
	{
		static GraphicsDevice instance;
		return instance;
	}

	bool Init(
		HWND hwnd,
		UINT resolutionW = ConstWindow::ScreenW,
		UINT resolutionH = ConstWindow::ScreenH
	);

	void BeginFrame();
	void EndFrame();

	//Getter
	ID3D11Device* GetDevice() const { return device.Get(); }
	ID3D11DeviceContext* GetContext() const { return context.Get(); }
	IDXGISwapChain* GetSwapChain() const { return swapChain.Get(); }
	ID3D11RenderTargetView* GetRTV() const { return renderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }
	UINT GetResolutionW() const { return resolutionW; }
	UINT GetResolutionH() const { return resolutionH; }

private:

	//DX11の各初期化処理
	bool CreateDevice();
	bool CreateSwapChain();
	bool CreateRenderTarget();
	bool CreateDepthStencil();
	void CreateViewport();
	void CreateScissorRect();

	//ウィンドウへのポインタ
	HWND hwnd = nullptr;

	//解像度
	UINT resolutionW = 0;
	UINT resolutionH = 0;

	//DX11固有のメンバ
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<ID3D11Texture2D> backBuffer;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> depthStencilView;

	D3D11_VIEWPORT viewport{};
	D3D11_RECT scissor{};

};