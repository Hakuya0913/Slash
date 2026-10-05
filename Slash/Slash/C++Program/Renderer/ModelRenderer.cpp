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

	if (ReadShaderFile(L"CSO/ModelVS.cso", vsData) == false)
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

	if (ReadShaderFile(L"CSO/ModelPS.cso", psData) == false)
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

	return true;

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
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
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

	ComPtr<IWICBitmapDecoder> decoder;

	hr = factory->CreateDecoderFromStream(
		stream.Get(),
		nullptr,
		WICDecodeMetadataCacheOnLoad,
		decoder.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	ComPtr<IWICBitmapFrameDecode> frame;

	hr = decoder->GetFrame(0, frame.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	UINT sorceWidth = 0;
	UINT sourceHeight = 0;

	hr = frame->GetSize(&sorceWidth, &sourceHeight);

	if (FAILED(hr))
	{
		return false;
	}

	//RGBAに変換
	ComPtr<IWICFormatConverter> converter;

	hr = factory->CreateFormatConverter(converter.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	hr = converter->Initialize(
		frame.Get(),
		GUID_WICPixelFormat32bppRGBA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0,
		WICBitmapPaletteTypeCustom
	);

	if (FAILED(hr))
	{
		return false;
	}

	width = sorceWidth;
	height = sorceWidth;
	rowPitch = width * 4;

	const size_t imageSize = rowPitch * height;

	pixelData.resize(imageSize);

	hr = converter->CopyPixels(
		nullptr,
		rowPitch,
		static_cast<UINT>(imageSize),
		pixelData.data()
	);

	if (FAILED(hr))
	{
		return false;
	}

	return true;

}

bool ModelRenderer::CreateConstantBuffers()
{

	//TransformData用の定数バッファ作成
	{

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(TransformData);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr;

		hr = device->CreateBuffer(
			&bufferDesc,
			nullptr,
			transformBuffer.GetAddressOf()
		);

		if (FAILED(hr))
		{
			return false;
		}

		bufferDesc.ByteWidth = sizeof(MaterialData);

		hr = device->CreateBuffer(
			&bufferDesc,
			nullptr,
			materialBuffer.GetAddressOf()
		);

		if (FAILED(hr))
		{
			return false;
		}

	}

	//MaterialData用の定数バッファ作成
	{

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(MaterialData);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		
		HRESULT hr;
		
		hr = device->CreateBuffer(
			&bufferDesc,
			nullptr,
			materialBuffer.GetAddressOf()
		);

		if (FAILED(hr))
		{
			return false;
		}

	}
	
	return true;

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

	data.resize(static_cast<size_t>(size));

	file.seekg(0);

	if (!file.read(reinterpret_cast<char*>(data.data()), size))
	{

		data.clear();
		return false;

	}

	return true;

}

void ModelRenderer::UpdateTransformBuffer(const TransformComponent& transform)
{
	
	if (context == nullptr || camera == nullptr || transformBuffer == nullptr)
	{
		return;
	}

	XMMATRIX world = transform.GetWorldMatrix();
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjectionMatrix();

	XMStoreFloat4x4(&transformData.world, XMMatrixTranspose(world));
	XMStoreFloat4x4(&transformData.view, XMMatrixTranspose(view));
	XMStoreFloat4x4(&transformData.proj, XMMatrixTranspose(proj));

	D3D11_MAPPED_SUBRESOURCE mappedResource{};

	HRESULT hr;

	hr = context->Map(
		transformBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
	);

	if (FAILED(hr))
	{
		return;
	}

	std::memcpy(mappedResource.pData, &transformData, sizeof(TransformData));

	context->Unmap(transformBuffer.Get(), 0);

}

void ModelRenderer::UpdateMaterialBuffer(const Material& material)
{

	if (context == nullptr || materialBuffer == nullptr)
	{
		return;
	}

	materialData.baseColor = material.baseColor;
	materialData.metallic = material.metallic;
	materialData.roughness = material.roughness;
	materialData.ambientOcclusion = material.ambientOcclusion;
	materialData.emissiveColor = material.emissiveColor;
	materialData.emissiveStrength = material.emissiveStrength;

	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	HRESULT hr;

	hr = context->Map(
		materialBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
	);

	if (FAILED(hr))
	{
		return;
	}

	std::memcpy(mappedResource.pData, &materialData, sizeof(MaterialData));
	context->Unmap(materialBuffer.Get(), 0);

}

void ModelRenderer::Render(
	const ModelComponent& model,
	const TransformComponent& transform
)
{

	if (context == nullptr || camera == nullptr)
	{
		return;
	}

	if (vertexShader == nullptr || pixelShader == nullptr || inputLayout == nullptr)
	{
		return;
	}

	UpdateTransformBuffer(transform);

	//描画の必須要素セット
	context->VSSetShader(vertexShader.Get(), nullptr, 0);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	context->IASetInputLayout(inputLayout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Shaderのセット

	ID3D11Buffer* vsConstantBuffers[] = { transformBuffer.Get() };
	context->VSSetConstantBuffers(0, 1, vsConstantBuffers);

	ID3D11SamplerState* samplers[] = { samplerState.Get() };
	context->PSSetSamplers(0, 1, samplers);

	const size_t meshCount = model.GetMeshCount();

	for (size_t i = 0; i < meshCount; ++i)
	{
		
		const MeshResource& resource = meshResources[i];

		if (resource.vertexBuffer == nullptr || resource.indexBuffer == nullptr ||
			resource.indexCount == 0)
		{
			continue;
		}

		//Materialインデックスが無効か
		if (resource.materialIndex >= model.GetMaterialCount())
		{
			continue;
		}

		const Material& material = model.GetMaterial(resource.materialIndex);

		UpdateMaterialBuffer(material);

		ID3D11Buffer* psConstantBuffers[] = { materialBuffer.Get() };

		/*
		//MaterialBufferを強制変更し、試験
		MaterialData testMaterialData{};
		testMaterialData.baseColor = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f); // 赤色に変更
		context->UpdateSubresource(materialBuffer.Get(), 0, nullptr, &testMaterialData, 0, 0);
		//試験コード終了
		*/

		context->PSSetConstantBuffers(1, 1, psConstantBuffers);

		//VertexBufferセット
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		ID3D11Buffer* vertexBuffers[] = { resource.vertexBuffer.Get() };

		context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

		//IndexBufferセット
		context->IASetIndexBuffer(resource.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		/*
		
		今回はBaseColorTextureのみを使用するので、別のテクスチャ情報も使用するなら
		処理追加の必要あり
		
		*/
		
		ID3D11ShaderResourceView* shaderResourceViews[] = { nullptr };
		const TextureReference& textureReference = material.baseColorTexture;

		if (textureReference.IsEmbedded() == true)
		{

			const int32_t textureIndex = textureReference.embeddedTextureIndex;
			
			if (textureIndex >= 0 &&
				static_cast<size_t>(textureIndex) < textureResources.size())
			{

				const TextureResource& texture = textureResources[textureIndex];
				shaderResourceViews[0] = texture.shaderResourceView.Get();

			}

			context->PSSetShaderResources(0, 1, shaderResourceViews);

			//DrawCall
			context->DrawIndexed(resource.indexCount, 0, 0);

		}

	}

	//後の描画処理に影響を与えないように、SRVを削除
	ID3D11ShaderResourceView* nullSRV[] = { nullptr };

	context->PSSetShaderResources(0, 1, nullSRV);

}