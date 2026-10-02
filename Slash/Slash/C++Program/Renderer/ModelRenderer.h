#pragma once

/*

FBXモデルを表示する

*/

#include<d3d11.h>
#include<vector>
#include<cstdint>
#include<DirectXMath.h>

#include"RendererStructure.h"
#include"../Utility/ComPtr.h"
#include"../Camera/Camera.h"
#include"../Component/ModelComponent.h"
#include"../Component/TransformComponent.h"

class ModelRenderer
{
public:

	ModelRenderer() = default;
	~ModelRenderer() = default;

	bool Init(ID3D11Device* device, ID3D11DeviceContext* context, Camera& camera);

	bool CreateModelResource(const ModelComponent& model);

	void Render(
		const ModelComponent& model,
		const TransformComponent& transform
	);

private:

	//各初期化関数
	bool CreateMeshResource(const Mesh& mesh, MeshResource& resource);
	bool CreateVertexBuffer(const Mesh& mesh, MeshResource& resource);
	bool CreateIndexBuffer(const Mesh& mesh, MeshResource& resource);
	bool CreateTextureResources(const ModelComponent& model);
	bool CreateTextureResource(const EmbeddedTexture& embeddedTexture, uint32_t embeddedTextureIndex);
	bool DecodeEmbeddedTexture(
		const EmbeddedTexture& embeddedTexture,
		std::vector<uint8_t>& pixelData,
		UINT& width, UINT& height, UINT& rowPitch
	);
	bool CreateConstantBuffers();

	//Shader読み込み
	bool ReadShaderFile(const wchar_t* filePath, std::vector<uint8_t>& data);

	//更新処理
	void UpdateTransformBuffer(const TransformComponent& transform);
	void UpdateMaterialBuffer(const Material& material);

	//DX11Object
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	//カメラへの参照
	Camera* camera = nullptr;

	//モデルリソース
	std::vector<MeshResource> meshResources;

	//定数バッファ
	ComPtr<ID3D11Buffer> transformBuffer;
	ComPtr<ID3D11Buffer> materialBuffer;

	TransformData transformData{};
	MaterialData materialData{};

	//テクスチャ
	std::vector<TextureResource> textureResources;

	//Shader
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;

	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11SamplerState> samplerState;

};