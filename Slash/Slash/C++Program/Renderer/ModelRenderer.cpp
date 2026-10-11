#include"ModelRenderer.h"
#include<d3dcompiler.h>
#include<wincodec.h>
#include<fstream>
#include<cstring>
#include<memory>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"windowscodecs.lib")

#ifdef _DEBUG

#include<sstream>

#endif

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

	if (CreateRasterizerState() == false)
	{
		return false;
	}

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

bool ModelRenderer::CreateRasterizerState()
{
	
	D3D11_RASTERIZER_DESC desc{};
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	HRESULT hr;

	hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());

	return SUCCEEDED(hr);

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
	const ModelResource& resource,
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

	//描画の必須要素セット
	context->VSSetConstantBuffers(0, 1, transformBuffer.GetAddressOf());
	context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	context->RSSetState(rasterizerState.Get());

	const size_t meshCount = resource.GetMeshCount();

	for (size_t i = 0; i < meshCount; ++i)
	{
		
		const MeshResource& meshResource = resource.GetMeshResource(i);

		if (meshResource.vertexBuffer == nullptr || meshResource.indexBuffer == nullptr ||
			meshResource.indexCount == 0)
		{
			continue;
		}

		//Materialインデックスが無効か
		if (meshResource.materialIndex >= model.GetMaterialCount())
		{
			continue;
		}

		const Material& material = model.GetMaterial(meshResource.materialIndex);

		UpdateMaterialBuffer(material);

		/*
		//MaterialBufferを強制変更し、試験
		MaterialData testMaterialData{};
		testMaterialData.baseColor = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f); // 赤色に変更
		context->UpdateSubresource(materialBuffer.Get(), 0, nullptr, &testMaterialData, 0, 0);
		//試験コード終了
		*/

		context->PSSetConstantBuffers(1, 1, materialBuffer.GetAddressOf());

		//VertexBufferセット
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		ID3D11Buffer* vertexBuffers[] = { meshResource.vertexBuffer.Get() };

		context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

		//IndexBufferセット
		context->IASetIndexBuffer(meshResource.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

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
				static_cast<size_t>(textureIndex) < resource.GetTextureCount())
			{

				const TextureResource& texture = resource.GetTextureResource(textureIndex);
				shaderResourceViews[0] = texture.shaderResourceView.Get();

			}

			context->PSSetShaderResources(0, 1, shaderResourceViews);

			//DrawCall
			context->DrawIndexed(meshResource.indexCount, 0, 0);

		}

	}

	//後の描画処理に影響を与えないように、SRVを削除
	ID3D11ShaderResourceView* nullSRV[] = { nullptr };

	context->PSSetShaderResources(0, 1, nullSRV);

}