#include"GraphicsDevice.h"

bool GraphicsDevice::Init(HWND hwnd, UINT resolutionW, UINT resolutionH)
{

	this->hwnd = hwnd;
	this->resolutionW = resolutionW;
	this->resolutionH = resolutionH;

	if (CreateDevice() == false)
	{
		return false;
	}

	if (CreateSwapChain() == false)
	{
		return false;
	}

	if (CreateRenderTarget() == false)
	{
		return false;
	}

	if (CreateDepthStencil() == false)
	{
		return false;
	}

	CreateViewport();
	CreateScissorRect();

	return true;

}

bool GraphicsDevice::CreateDevice()
{

	UINT creationFlag = 0;

#ifdef _DEBUG

	creationFlag |= D3D11_CREATE_DEVICE_DEBUG;

#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr;

	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlag,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		&featureLevel,
		context.GetAddressOf()
	);

	if (FAILED(hr))
	{

		//D3D_FEATURE_LEVEL_11が使用できない環境への対応
		if (hr == E_INVALIDARG)
		{

			D3D_FEATURE_LEVEL fallbackLevels[] =
			{
				D3D_FEATURE_LEVEL_11_0
			};

			hr = D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				creationFlag,
				fallbackLevels,
				_countof(fallbackLevels),
				D3D11_SDK_VERSION,
				device.GetAddressOf(),
				&featureLevel,
				context.GetAddressOf()
			);

		}

	}

	if (FAILED(hr))
	{
		return false;
	}

	return true;

}

bool GraphicsDevice::CreateSwapChain()
{

	ComPtr<IDXGIDevice> dxgiDevice;

	HRESULT hr;

	hr = device->QueryInterface(IID_PPV_ARGS(dxgiDevice.GetAddressOf()));

	if (FAILED(hr))
	{
		return false;
	}

	ComPtr<IDXGIAdapter> adapter;
	
	hr = dxgiDevice->GetAdapter(adapter.GetAddressOf());

	if (FAILED(hr))
	{
		return true;
	}

	ComPtr<IDXGIFactory> factory;

	hr = adapter->GetParent(IID_PPV_ARGS(factory.GetAddressOf()));

	if (FAILED(hr))
	{
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferCount = ConstD3D::BufferringCount;
	swapChainDesc.BufferDesc.Width = resolutionW;
	swapChainDesc.BufferDesc.Height = resolutionH;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = ConstD3D::RefreshNumrator;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = ConstD3D::RefreshDenomirator;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	
	hr = factory->CreateSwapChain(
		device.Get(),
		&swapChainDesc,
		swapChain.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	/*
	ALT + Enterでフルスクリーン切り替えを
	無効にした場合はコメントアウト外してね
	
	factory->MakeWindowAssociation(
		hwnd,
		DXGI_MWA_NO_ALT_ENTER
	);

	*/

	return true;

}

bool GraphicsDevice::CreateRenderTarget()
{

	if (swapChain == nullptr)
	{
		return false;
	}

	HRESULT hr;

	hr = swapChain->GetBuffer(
		0,
		IID_PPV_ARGS(backBuffer.GetAddressOf())
	);

	if (FAILED(hr))
	{
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(
		backBuffer.Get(),
		&rtvDesc,
		renderTargetView.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	return true;

}

bool GraphicsDevice::CreateDepthStencil()
{

	D3D11_TEXTURE2D_DESC depthDesc{};
	depthDesc.Width = resolutionW;
	depthDesc.Height = resolutionH;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	HRESULT hr;

	hr = device->CreateTexture2D(
		&depthDesc,
		nullptr,
		depthStencilBuffer.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(
		depthStencilBuffer.Get(),
		&dsvDesc,
		depthStencilView.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	return true;

}

void GraphicsDevice::CreateViewport()
{

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	viewport.Width = static_cast<float>(resolutionW);
	viewport.Height = static_cast<float>(resolutionH);

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

}

void GraphicsDevice::CreateScissorRect()
{

	scissor.left = 0;
	scissor.top = 0;

	scissor.right = static_cast<LONG>(resolutionW);
	scissor.bottom = static_cast<LONG>(resolutionH);

}

void GraphicsDevice::BeginFrame()
{

	if (context == nullptr)
	{
		return;
	}

	//RenderTarget,DepthStencilを設定
	context->OMSetRenderTargets(
		1,
		renderTargetView.GetAddressOf(),
		depthStencilView.Get()
	);

	//Viewport設定
	context->RSSetViewports(
		1,
		&viewport
	);

	//Scissor設定
	context->RSSetScissorRects(
		1,
		&scissor
	);

	//RenderTargetClear
	context->ClearRenderTargetView(
		renderTargetView.Get(),
		ConstD3D::ClearColor
	);

	//DepthStencilをクリア
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

}

void GraphicsDevice::EndFrame()
{

	if (swapChain == nullptr)
	{
		return;
	}

	swapChain->Present(
		1,
		0
	);

}