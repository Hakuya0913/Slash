#include"ModelRenderer.h"
#include<d3dcompiler.h>
#include<wincodec.h>
#include<fstream>
#include<cstring>
#include<memory>

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

	//VS読み込み
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

	//PS読み込み
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

	//InputLayout作成
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

	//SamplerState作成
	D3D11_SAMPLER_DESC samplerDesc{};

	samplerDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias		= 0.0f;
	samplerDesc.MaxAnisotropy	= 1;
	samplerDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD			= 0.0f;
	samplerDesc.MaxLOD			= D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	
	if (FAILED(hr))
	{
		return false;
	}

	//ConstantBuffer作成
	if (CreateConstantBuffers() == false)
	{
		return false;
	}

	return true;
}

bool ModelRenderer::CreateModelResource(const ModelComponent& model)
{
	
	if (device == nullptr)
	{
		return false;
	}

	meshResources.clear();
	textureResources.clear();

	const size_t meshCount = model.GetMeshCount();

	meshResources.resize(meshCount);

	for (size_t i = 0; i < meshCount; ++i)
	{

		const Mesh& mesh = model.GetMesh(i);

		if (CreateMeshResource(mesh, meshResources[i]) == false)
		{
			return false;
		}

	}

	//テクスチャリソース作成
	if (CreateTextureResources(model) == false)
	{
		return false;
	}

}

bool ModelRenderer::CreateMeshResource(const Mesh& mesh, MeshResource& resource)
{

	resource.indexCount = static_cast<uint32_t>(mesh.indices.size());

	if (CreateVertexBuffer(mesh, resource) == false)
	{
		return false;
	}

	if (CreateIndexBuffer(mesh, resource) == false)
	{
		return false;
	}

	return false;

}

bool ModelRenderer::CreateVertexBuffer(const Mesh& mesh, MeshResource& resource)
{

	if (mesh.vertices.empty() == true)
	{
		return false;
	}

	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = mesh.vertices.data();

	HRESULT hr;

	hr = device->CreateBuffer(
		&bufferDesc,
		&initialData,
		resource.vertexBuffer.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	return true;

}

bool ModelRenderer::CreateIndexBuffer(const Mesh& mesh, MeshResource& resource)
{

	if (mesh.indices.empty() == true)
	{
		return false;
	}

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = mesh.indices.data();

	HRESULT hr;

	hr = device->CreateBuffer(
		&bufferDesc,
		&initialData,
		resource.indexBuffer.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	return true;

}

bool ModelRenderer::CreateTextureResources(const ModelComponent& model)
{

	const size_t textureCount = model.GetEmbeddedTextureCount();

	for (size_t i = 0; i < textureCount; ++i)
	{

		const EmbeddedTexture& embeddedTexture = model.GetEmbeddedTexture(i);

		if (CreateTextureResource(embeddedTexture, static_cast<uint32_t>(i)) == false)
		{

			return false;

		}

	}

	return true;

}

bool ModelRenderer::CreateTextureResource(const EmbeddedTexture& embeddedTexture, uint32_t embeddedTextureIndex)
{

	std::vector<uint8_t> pixelData;

	UINT width = 0;
	UINT height = 0;
	UINT rowPitch = 0;

	bool funcResult;

	funcResult = DecodeEmbeddedTexture(
		embeddedTexture,
		pixelData,
		width, height, rowPitch
	);

	if (funcResult == false)
	{
		return false;
	}

	if (pixelData.empty() == true ||
		width == 0 || height == 0)
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureData{};
	textureData.pSysMem = pixelData.data();
	textureData.SysMemPitch = rowPitch;

	TextureResource resource;
	HRESULT hr;

	hr = device->CreateTexture2D(
		&textureDesc,
		&textureData,
		resource.texture.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	//ShaderResourceView作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(
		resource.texture.Get(),
		&srvDesc,
		resource.shaderResourceView.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	resource.embeddedTextureIndex = static_cast<int32_t>(embeddedTextureIndex);
	textureResources.emplace_back(std::move(resource));

	return true;

}

bool ModelRenderer::DecodeEmbeddedTexture(
	const EmbeddedTexture& embeddedTexture,
	std::vector<uint8_t>& pixelData,
	UINT& width, UINT& height, UINT& rowPitch
)
{

	pixelData.clear();

	width = 0;
	height = 0;
	rowPitch = 0;

	if (embeddedTexture.data.empty() == true)
	{
		return false;
	}

	ComPtr<IWICImagingFactory> factory;
	HRESULT hr;

	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(factory.GetAddressOf())
	);

	if (FAILED(hr))
	{
		return false;
	}

	ComPtr<IWICStream> stream;

	hr = factory->CreateStream(stream.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	hr = stream->InitializeFromMemory(
		const_cast<BYTE*>(reinterpret_cast<const BYTE*>(embeddedTexture.data.data())),
		static_cast<DWORD>(embeddedTexture.data.size())
	);

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