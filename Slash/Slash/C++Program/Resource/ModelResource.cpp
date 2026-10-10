#include"ModelResource.h"
#include<wincodec.h>

#pragma comment(lib,"windowscodecs.lib")

bool ModelResource::Create(ID3D11Device* device, const ModelComponent& model)
{

	if (device == nullptr)
	{
		return false;
	}

	Release();

	if (CreateMeshResources(device, model) == false)
	{
		Release();
		return false;
	}

	if (CreateTextureResources(device, model) == false)
	{
		Release();
		return false;
	}

	return true;

}

void ModelResource::Release()
{

	meshResources.clear();
	textureResources.clear();

}

bool ModelResource::CreateMeshResources(
	ID3D11Device* device,
	const ModelComponent& model
)
{

	meshResources.reserve(model.GetMeshCount());

	for (size_t i = 0; i < model.GetMeshCount(); ++i)
	{
		if (!CreateMeshResource(device, model, i))
		{
			return false;
		}
	}

	return true;
}

bool ModelResource::CreateMeshResource(
	ID3D11Device* device,
	const ModelComponent& model,
	size_t meshIndex
)
{

	if (meshIndex >= model.GetMeshCount())
	{
		return false;
	}

	const auto& mesh = model.GetMesh(meshIndex);

	MeshResource resource{};

	resource.indexCount = static_cast<uint32_t>(mesh.indices.size());
	resource.materialIndex = mesh.materialIndex;

	if (!CreateVertexBuffer(
		device,
		mesh.vertices,
		resource))
	{
		return false;
	}

	if (!CreateIndexBuffer(
		device,
		mesh.indices,
		resource))
	{
		return false;
	}

	meshResources.push_back(std::move(resource));

	return true;
}

bool ModelResource::CreateVertexBuffer(
	ID3D11Device* device,
	const std::vector<Vertex>& vertices,
	MeshResource& resource
)
{
	if (device == nullptr || vertices.empty())
	{
		return false;
	}

	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(Vertex) * vertices.size()
			);

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data{};

	data.pSysMem = vertices.data();

	HRESULT hr = device->CreateBuffer(
		&bufferDesc,
		&data,
		resource.vertexBuffer.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool ModelResource::CreateIndexBuffer(
	ID3D11Device* device,
	const std::vector<uint32_t>& indices,
	MeshResource& resource
)
{
	if (device == nullptr || indices.empty())
	{
		return false;
	}

	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.ByteWidth =
		static_cast<UINT>(
			sizeof(uint32_t) * indices.size()
			);

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data{};

	data.pSysMem = indices.data();

	HRESULT hr = device->CreateBuffer(
		&bufferDesc,
		&data,
		resource.indexBuffer.GetAddressOf()
	);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool ModelResource::CreateTextureResources(
	ID3D11Device* device,
	const ModelComponent& model
)
{

	textureResources.reserve(model.GetEmbeddedTextureCount());

	for (size_t i = 0; i < model.GetEmbeddedTextureCount(); ++i)
	{
		if (!CreateTextureResource(
			device,
			model,
			i))
		{
			return false;
		}
	}

	return true;
}

bool ModelResource::CreateTextureResource(
	ID3D11Device* device,
	const ModelComponent& model,
	size_t textureIndex
)
{
	std::vector<uint8_t> pixelData;

	UINT width = 0;
	UINT height = 0;
	UINT rowPitch = 0;

	bool funcResult;

	const EmbeddedTexture& embeddedTexture = model.GetEmbeddedTexture(textureIndex);

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

	resource.embeddedTextureIndex = static_cast<int32_t>(textureIndex);
	textureResources.emplace_back(std::move(resource));

	return true;
}

bool ModelResource::DecodeEmbeddedTexture(
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
	height = sourceHeight;
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