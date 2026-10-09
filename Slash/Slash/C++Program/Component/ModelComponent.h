#pragma once

/*

モデルの情報を持つコンポーネント

*/

#include<DirectXMath.h>
#include<vector>
#include<cstdint>
#include<string>
#include<assimp/scene.h>
#include"ModelStructure.h"

class ModelComponent
{
public:

	ModelComponent() = default;
	~ModelComponent() = default;

	//モデルの読み込み
	bool Load(const std::string& filePath);

	//Getter
			size_t	GetMeshCount()			const	{ return meshes.size(); }
	const	Mesh&	GetMesh(size_t index)	const	{ return meshes.at(index); }

			size_t	GetNodeCount()			const	{ return nodes.size(); }
	const	Node&	GetNode(size_t index)	const	{ return nodes.at(index); }

			size_t	GetBoneCount()			const	{ return bones.size(); }
	const	Bone&	GetBone(size_t index)	const	{ return bones.at(index); }

			size_t		GetMaterialCount()		  const { return materials.size(); }
	const	Material&	GetMaterial(size_t index) const { return materials.at(index); }

			size_t			 GetEmbeddedTextureCount()		  const { return embeddedTextures.size(); }
	const	EmbeddedTexture& GetEmbeddedTexture(size_t index) const { return embeddedTextures.at(index); }
	
	const std::vector<Mesh>& GetMeshes() const { return meshes; }
	const std::vector<Node>& GetNodes() const { return nodes; }
	const std::vector<Bone>& GetBones() const { return bones; }
	const std::vector<Material>& GetMaterials() const { return materials; }
	const std::vector<EmbeddedTexture>& GetEmbeddedTextures() const { return embeddedTextures; }

private:

	//Assimpを使用した各ロード処理
	bool LoadScene(	const aiScene* scene);
	bool LoadNodes(	const aiNode*  node, int32_t parentIndex);
	bool LoadMeshes(const aiScene* scene);
	bool LoadMesh(	const aiMesh* mesh);
	bool LoadBones(	const aiMesh*  mesh, Mesh& result);
	bool LoadMaterials(const aiScene* scene);
	bool LoadMaterial(const aiScene* scene, const aiMaterial* material);
	bool LoadEmbeddedTextures(const aiScene* scene);

	TextureReference GetTextureReference(const aiScene* scene, const aiMaterial* material, aiTextureType type);

	//Utility
	DirectX::XMFLOAT4X4 ConvertMatrix(const aiMatrix4x4& matrix) const;
	uint32_t			FindNodeIndex(const std::string& name) const;

	std::vector<Mesh> meshes;
	std::vector<Node> nodes;
	std::vector<Bone> bones;

	std::vector<Material>		 materials;
	std::vector<EmbeddedTexture> embeddedTextures;

};
