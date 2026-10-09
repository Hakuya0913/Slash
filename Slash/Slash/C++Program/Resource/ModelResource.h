#pragma once

/*

ロードしたモデルのGPUリソースを保持する


*/

#include<d3d11.h>
#include<vector>
#include<cstdint>
#include"../Utility/ComPtr.h"
#include"../Component/ModelComponent.h"
#include"../Renderer/RendererStructure.h"

class ModelResource
{
public:

	ModelResource() = default;
	~ModelResource() = default;

	ModelResource(const ModelResource&) = delete;
	ModelResource& operator = (const ModelResource&) = delete;

	bool Create(ID3D11Device* device, const ModelComponent& model);
	void Release();

	bool IsValid() const { return (meshResources.empty() == false); }

	const std::vector<MeshResource>& GetMeshResources() const { return meshResources; }
	const std::vector<TextureResource>& GetTextureResource() const { return textureResources; }
	const MeshResource& GetMeshResource(size_t index) const { return meshResources[index]; }
	const TextureResource& GetTextureResource(size_t index) const { return textureResources[index]; }
	size_t GetMeshCount() const { return meshResources.size(); }
	size_t GetTextureCount() const { return textureResources.size(); }

private:

	bool CreateMeshResources(ID3D11Device* device, const ModelComponent& model);
	bool CreateMeshResource(ID3D11Device* device, const ModelComponent& model, size_t meshIndex);
	bool CreateVertexBuffer(ID3D11Device* device, const std::vector<Vertex>& vertices, MeshResource& resource);
	bool CreateIndexBuffer(ID3D11Device* device, const std::vector<uint32_t>& indices, MeshResource& resource);
	bool CreateTextureResources(ID3D11Device* device, const ModelComponent& model);
	bool CreateTextureResource(ID3D11Device* device, const ModelComponent& model, size_t textureIndex);

	bool DecodeEmbeddedTexture(
		const EmbeddedTexture& embeddedTexture,
		std::vector<uint8_t>& pixelData,
		UINT& width, UINT& height, UINT& rowPitch
	);

	std::vector<MeshResource> meshResources;
	std::vector<TextureResource> textureResources;

};