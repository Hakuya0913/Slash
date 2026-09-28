#pragma once

/*

FBXモデルを表示する

*/

#include<d3d11.h>
#include<vector>
#include<cstdint>
#include<array>
#include<unordered_map>
#include<wincodec.h>
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
		ID3D11DeviceContext* context,
		const ModelComponent& model,
		const TransformComponent& transform
	);

private:

	//各初期化関数
	bool CreateMeshResource(const Mesh& mesh, MeshResource& resource);
	bool CreateVertexBuffer(const Mesh& mesh, MeshResource& resource);
	bool CreateIndexBuffer(const Mesh& mesh, MeshResource& resource);
	bool CreateTextureResources(const ModelComponent& model);
	bool CreateTextureResource(const EmbeddedTexture& embeddedTxture, uint32_t embeddedTextureIndex);
	bool DecodeEmbeddedTexture(
		const EmbeddedTexture& embeddedTexture,
		std::vector<uint8_t>& pixelData,
		UINT& width, UINT& height, UINT& rowPitch
	);
	bool CreateConstantBuffers();

	//更新処理
	void UpdateTransformBuffer(const TransformComponent& transform);
	void UpdateMaterialBuffer(const Material& material);

	//DX11Object
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	//カメラへの参照
	Camera* camera = nullptr;

	//モデルリソース
	std::vector<MeshResource> meshResource;

	//定数バッファ
	ComPtr<ID3D11Buffer> transformBuffer;
	ComPtr<ID3D11Buffer> materialBUffer;

	TransformData transfromData{};
	MaterialData materialData{};

	//テクスチャ
	std::vector<TextureResource> textureResources;

};