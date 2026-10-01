#include"ModelRenderer.h"
#include<d3dcompiler.h>
#include<wincodec.h>
#include<fstream>
#include<limits>
#include<cstring>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"windowscodecs.lib")

using namespace DirectX;
using namespace DirectX::SimpleMath;

bool ModelRenderer::Init(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	Camera& camera
)
{

	if (device == nullptr || context == nullptr)
	{
		return false;
	}

	this->device = device;
	this->context = context;
	this->camera = &camera;

	HRESULT hr;

	//VSì«Ç›çûÇ›
	std::vector<uint8_t> vsData;

	if (ReadShaderFile(L"Shader/ModelVS.cso", vsData) == false)
	{
		return false;
	}

	hr = device->CreateVertexShader(
		vsData.data(),
		vsData.size(),
		nullptr,
		vertexShader.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	//PSì«Ç›çûÇ›
	std::vector<uint8_t> psData;

	if (ReadShaderFile(L"Shader/ModelPS.cso", psData) == false)
	{
		return false;
	}

	hr = device->CreatePixelShader(
		psData.data(),
		psData.size(),
		nullptr,
		pixelShader.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	//InputLayoutçÏê¨
	D3D11_INPUT_ELEMENT_DESC inputElements[] =
	{

		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	hr = device->CreateInputLayout(
		inputElements,
		_countof(inputElements),
		vsData.data(),
		vsData.size(),
		inputLayout.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	//SamplerStateçÏê¨
	D3D11_SAMPLER_DESC samplerDesc{};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	
	if (FAILED(hr))
	{
		return false;
	}

	//ConstantBufferçÏê¨
	if (CreateConstantBuffers() == false)
	{
		return false;
	}

	return true;
}

bool ModelRenderer::CreateModelResource(const ModelComponent& model)
{
	
	meshResource.clear();
	textureResources.clear();

}

bool ModelRenderer::ReadShaderFile(const wchar_t* filePath, std::vector<uint8_t>& data)
{
	
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	
	if (file.is_open() == false)
	{
		return false;
	}

	std::streamsize size = file.tellg();

	if (size <= 0)
	{
		return false;
	}

	file.seekg(0, std::ios::beg);
	data.resize(static_cast<size_t>(size));

	if (!file.read(reinterpret_cast<char*>(data.data()), size))
	{
		return false;
	}

	return true;

}