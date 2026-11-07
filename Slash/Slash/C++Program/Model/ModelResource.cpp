#include"ModelResource.h"

#define NOMINMAX
#include<Windows.h>

#include<wincodec.h>
#include<limits>

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

	
	if (device == nullptr)
	{
		return false;
	}

	if (meshIndex >= model.GetMeshCount())
	{
		return false;
	}

	const auto& mesh = model.GetMesh(meshIndex);

	MeshResource resource{};

	resource.indexCount = static_cast<uint32_t>(mesh.indices.size());
	resource.materialIndex = mesh.materialIndex;
	resource.boneIndices = mesh.boneIndices;

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

	if (device == nullptr)
	{
		return false;
	}

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

	if (device == nullptr ||
		textureIndex >= model.GetEmbeddedTextureCount())
	{
		return false;
	}

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

	//非圧縮画像のデコード処理
	if (embeddedTexture.isCompressed == false)
	{

		return DecodeUnCompressedTexture(
			embeddedTexture,
			pixelData,
			width, height, rowPitch
		);

	}

	if (embeddedTexture.data.size() > (std::numeric_limits<DWORD>::max)())
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

	if (FAILED(hr))
	{
		return false;
	}

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

	UINT sourceWidth = 0;
	UINT sourceHeight = 0;

	hr = frame->GetSize(&sourceWidth, &sourceHeight);

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

	if (sourceWidth == 0 || sourceHeight == 0)
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

	//画像サイズ計算
	const uint64_t calculateRowPitch = sourceWidth * 4;
	const uint64_t calculateImageSize = calculateRowPitch * sourceHeight;

	if (calculateRowPitch > (std::numeric_limits<UINT>::max)() ||
		calculateImageSize > (std::numeric_limits<UINT>::max)())
	{
		return false;
	}

	width = sourceWidth;
	height = sourceHeight;
	rowPitch = calculateRowPitch;

	pixelData.resize(calculateImageSize);

	hr = converter->CopyPixels(
		nullptr,
		rowPitch,
		calculateImageSize,
		pixelData.data()
	);

	if (FAILED(hr))
	{

		pixelData.clear();
		width = 0;
		height = 0;
		rowPitch = 0;

		return false;

	}

	return true;

}

bool ModelResource::DecodeUnCompressedTexture(
	const EmbeddedTexture& embeddedTexture,
	std::vector<uint8_t>& pixelData,
	UINT& width, UINT& height, UINT& rowPitch
)
{

	pixelData.clear();

	width = 0;
	height = 0;
	rowPitch = 0;

	if (embeddedTexture.width == 0 ||
		embeddedTexture.height == 0)
	{
		return false;
	}

	if (embeddedTexture.width > UINT_MAX / 4)
	{
		return false;
	}

	width = embeddedTexture.width;
	height = embeddedTexture.height;
	rowPitch = width * 4;

	const uint64_t imageSize = static_cast<uint64_t>(rowPitch) * height;

	if (imageSize > (std::numeric_limits<size_t>::max)())
	{
		return false;
	}

	if (embeddedTexture.data.size() < imageSize)
	{
		return false;
	}

	pixelData.resize(imageSize);

	const size_t pixelCount = width * height;

	for (size_t i = 0; i < pixelCount; ++i)
	{

		const size_t offset = i * 4;

		pixelData[offset + 0] = embeddedTexture.data[offset + 2];
		pixelData[offset + 1] = embeddedTexture.data[offset + 1];
		pixelData[offset + 2] = embeddedTexture.data[offset	+ 0];
		pixelData[offset + 3] = embeddedTexture.data[offset + 3];

	}

	return true;

}