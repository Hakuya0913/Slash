#define NOMINMAX

#include"ModelComponent.h"
#include<Windows.h>
#include<assimp/Importer.hpp>
#include<assimp/postprocess.h>
#include<algorithm>
#include<cstring>

bool ModelComponent::Load(const std::string& filePath)
{

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(
		filePath,
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded
	);

	if (scene == nullptr)
	{
		return false;
	}

	if ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0)
	{
		return false;
	}

	if (!scene->mRootNode)
	{
		return false;
	}

	return LoadScene(scene);

}

bool ModelComponent::LoadScene(const aiScene* scene)
{

	if (scene == nullptr)
	{
		return false;
	}

	meshes.clear();
	nodes.clear();
	bones.clear();
	materials.clear();
	embeddedTextures.clear();

	if (LoadNodes(scene->mRootNode, -1) == false)
	{
		return false;
	}

	if (LoadEmbeddedTextures(scene) == false)
	{
		return false;
	}

	if (LoadMaterials(scene) == false)
	{
		return false;
	}

	if (LoadMeshes(scene) == false)
	{
		return false;
	}

	return true;

}

bool ModelComponent::LoadNodes(const aiNode* node, int32_t parentIndex)

{

	if (!node)
	{
		return false;
	}

	Node currentNode;
	currentNode.name			= node->mName.C_Str();
	currentNode.parentIndex		= parentIndex;
	currentNode.localTransform	= ConvertMatrix(node->mTransformation);

	uint32_t currentIndex = static_cast<uint32_t>(nodes.size());
	
	nodes.emplace_back(currentNode);

	//子Nodeを読み込む
	for (UINT i = 0; i < node->mNumChildren; ++i)
	{

		const aiNode* child = node->mChildren[i];

		int32_t childIndex = static_cast<int32_t>(nodes.size());

		if (!LoadNodes(child, currentIndex))
		{
			return false;
		}

		nodes[currentIndex].children.emplace_back(childIndex);

	}

	return true;

}

//Mesh読み込み
bool ModelComponent::LoadMeshes(const aiScene* scene)
{

	if (!scene)
	{
		return false;
	}

	meshes.reserve(scene->mNumMeshes);

	for (UINT i = 0; i < scene->mNumMeshes; ++i)
	{

		if (!LoadMesh(scene->mMeshes[i]))
		{

			return false;

		}

	}

	return true;

}

//1つのMesh読み込み
bool ModelComponent::LoadMesh(const aiMesh* mesh)
{

	if (mesh == nullptr)
	{
		return false;
	}

	Mesh result;
	result.name = mesh->mName.C_Str();

	//Vertex
	result.vertices.reserve(mesh->mNumVertices);
	result.boneWeights.reserve(mesh->mNumVertices);

	//Vertex
	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{

		Vertex vertex;

		//Position
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		//Normal
		if (mesh->HasNormals())
		{

			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;

		}

		//TexCoord
		if (mesh->HasTextureCoords(0))
		{

			vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = mesh->mTextureCoords[0][i].y;

		}

		//Tangent
		if (mesh->HasTangentsAndBitangents())
		{

			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;

		}

		result.vertices.emplace_back(vertex);

	}

	//Index
	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{

		const aiFace& face = mesh->mFaces[i];

		//三角形以外は想定しない
		if (face.mNumIndices != 3)
		{

			continue;

		}

		for (UINT j = 0; j < face.mNumIndices; ++j)
		{

			result.indices.emplace_back(face.mIndices[j]);

		}

	}

	//Material
	result.materialIndex = mesh->mMaterialIndex;

	//Bone
	if (mesh->HasBones())
	{

		if (LoadBones(mesh, result) == false)
		{
			return false;
		}

	}
	else
	{

		//Boneを持たないMeshでもVertex数に応じて確保
		result.boneWeights.resize(result.vertices.size());

	}

	meshes.emplace_back(std::move(result));

	return true;

}

bool ModelComponent::LoadBones(const aiMesh* mesh, Mesh& result)
{

	if (!mesh)
	{
		return false;
	}

	//Assimpから取得したBoneWeightを一時的にすべて保持
	std::vector<std::vector<BoneWeight>> temporaryWeights(result.vertices.size());

	for (UINT i = 0; i < mesh->mNumBones; ++i)
	{

		const aiBone* aiBone = mesh->mBones[i];

		if (aiBone == nullptr)
		{
			continue;
		}

		std::string boneName = aiBone->mName.C_Str();

		//すでに登録済みか検索
		uint32_t boneIndex = UINT32_MAX;

		for (uint32_t j = 0; j < bones.size(); ++j)
		{

			if (bones[j].name == boneName)
			{

				boneIndex = j;
				break;

			}

		}

		//新しいBone
		if (boneIndex == UINT32_MAX)
		{

			Bone bone;

			bone.name		= boneName;
			bone.nodeIndex  = FindNodeIndex(boneName);

			if (bone.nodeIndex == UINT32_MAX)
			{
				return false;
			}

			bone.offsetMatrix = ConvertMatrix(aiBone->mOffsetMatrix);
			boneIndex = static_cast<uint32_t>(bones.size());

			bones.emplace_back(bone);

		}

		//このMeshで使用するBoneとして登録
		result.boneIndices.emplace_back(boneIndex);

		//VertexWeightを一時保存
		for (UINT j = 0; j < aiBone->mNumWeights; ++j)
		{

			const aiVertexWeight& weight = aiBone->mWeights[j];

			if (weight.mVertexId >= result.vertices.size())
			{
				continue;
			}

			BoneWeight boneWeight;

			boneWeight.boneIndex = boneIndex;
			boneWeight.weight	 = weight.mWeight;

			temporaryWeights[weight.mVertexId].emplace_back(boneWeight);

		}

		//各VertexのNoneWeightを最大4本に制限
		result.boneWeights.resize(result.vertices.size());

		for (size_t vertexIndex = 0; vertexIndex < temporaryWeights.size(); ++vertexIndex)
		{

			auto& sourceWeights = temporaryWeights[vertexIndex];
			auto& destinationWeights = result.boneWeights[vertexIndex];

			//Weightの大きさ順に並べる
			std::sort(sourceWeights.begin(), sourceWeights.end(),
				[](const BoneWeight& a, const BoneWeight& b)
				{
					return a.weight > b.weight;
				}
			);

			//上位４つのボーンを採用
			const size_t count = std::min(sourceWeights.size(), static_cast<size_t>(Mesh::MaxBoneInfluences));

			float totalWeight = 0.0f;

			for (size_t i = 0; i < count; ++i)
			{

				destinationWeights[i] = sourceWeights[i];

				totalWeight += sourceWeights[i].weight;

			}

			//weightを正規化
			if (totalWeight > 0.0f)
			{

				for (size_t i = 0; i < count; ++i)
				{

					destinationWeights[i].weight /= totalWeight;

				}

			}

		}

	}

	return true;

}

uint32_t ModelComponent::FindNodeIndex(const std::string& name) const
{

	for (uint32_t i = 0; i < nodes.size(); ++i)
	{

		if (nodes[i].name == name)
		{

			return i;

		}

	}

	return UINT32_MAX;

}

bool ModelComponent::LoadEmbeddedTextures(const aiScene* scene)
{

	if (scene == nullptr)
	{
		return false;
	}

	embeddedTextures.reserve(scene->mNumTextures);

	for (UINT i = 0; i < scene->mNumTextures; ++i)
	{

		const aiTexture* aiTexture = scene->mTextures[i];

		if (aiTexture == nullptr)
		{
			continue;
		}

		EmbeddedTexture texture;

		texture.name = aiTexture->mFilename.C_Str();
		texture.width = aiTexture->mWidth;
		texture.height = aiTexture->mHeight;
		texture.formatHint = aiTexture->achFormatHint;

		//mHeightが0なら圧縮画像
		if (aiTexture->mHeight == 0)
		{

			texture.isCompressed = true;

			texture.data.resize(aiTexture->mWidth);

			if (texture.data.empty() == false)
			{
				
				std::memcpy(
					texture.data.data(),
					aiTexture->pcData,
					texture.data.size()
				);

			}

		}
		else
		{

			//非圧縮のaiTexel
			texture.isCompressed = false;

			const size_t dataSize = aiTexture->mWidth * aiTexture->mHeight * sizeof(aiTexel);

			texture.data.resize(dataSize);

			if (texture.data.empty() == false)
			{

				std::memcpy(
					texture.data.data(),
					aiTexture->pcData,
					dataSize
				);

			}

		}

		embeddedTextures.emplace_back(std::move(texture));

	}

	return true;

}

bool ModelComponent::LoadMaterials(const aiScene* scene)
{

	if (scene == nullptr)
	{
		return false;
	}

	materials.reserve(scene->mNumMaterials);

	for (UINT i = 0; i < scene->mNumMaterials; ++i)
	{

		//個々のマテリアル読み込み
		if (LoadMaterial(scene->mMaterials[i]) == false)
		{
			return false;
		}

	}

	return true;

}

bool ModelComponent::LoadMaterial(const aiMaterial* material)
{

	if (material == nullptr)
	{
		return false;
	}

	Material result;
	aiString materialName;
	aiReturn isSuccess = AI_FAILURE;	//各関数の成功判定保持

	isSuccess = material->Get(AI_MATKEY_NAME, materialName);
	if (isSuccess == AI_SUCCESS)
	{

		result.name = materialName.C_Str();

	}

	//BaseColor取得
	{

		aiColor4D baseColor;
		isSuccess = material->Get(AI_MATKEY_BASE_COLOR, baseColor);

		if (isSuccess == AI_SUCCESS)
		{

			result.baseColor.x = baseColor.r;
			result.baseColor.y = baseColor.g;
			result.baseColor.z = baseColor.b;
			result.baseColor.w = baseColor.a;

		}
		else
		{

			//PBRのBaseColorが取得できない場合
			//DiffuseColorを使用する
			aiColor4D diffuseColor;
			isSuccess = material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

			if (isSuccess == AI_SUCCESS)
			{


				result.baseColor.x = diffuseColor.r;
				result.baseColor.y = diffuseColor.g;
				result.baseColor.z = diffuseColor.b;
				result.baseColor.w = diffuseColor.a;

			}

		}

	}

	//Metallic取得
	{

		float metallic = 0.0f;
		isSuccess = material->Get(AI_MATKEY_METALLIC_FACTOR, metallic);

		if (isSuccess == AI_SUCCESS)
		{

			result.metallic = metallic;

		}

	}

	//Roughness
	{

		float roughness = 1.0f;
		isSuccess = material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);

		if (isSuccess == AI_SUCCESS)
		{

			result.roughness = roughness;

		}

	}

	//EmissiveColor取得
	{

		aiColor3D emissiveColor;
		isSuccess = material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);

		if (isSuccess == AI_SUCCESS)
		{

			result.emissiveColor.x = emissiveColor.r;
			result.emissiveColor.y = emissiveColor.g;
			result.emissiveColor.z = emissiveColor.b;

		}

	}

	//EmissiveStrength取得
	{

		float emissiveStrength = 1.0f;
		isSuccess = material->Get(AI_MATKEY_EMISSIVE_INTENSITY, emissiveStrength);

		if (isSuccess == AI_SUCCESS)
		{

			result.emissiveStrength = emissiveStrength;

		}

	}

	//各テクスチャリファレンス構造体の取得

	//BaseColorが取得出来ない場合DiffuseColorを使用する
	result.baseColorTexture = GetTextureReference(material, aiTextureType_BASE_COLOR);
	if (result.baseColorTexture.IsValid() == false)
	{

		result.baseColorTexture = GetTextureReference(material, aiTextureType_DIFFUSE);

	}

	result.normalTexture = GetTextureReference(material, aiTextureType_NORMALS);
	result.ambientOcclusionTexture = GetTextureReference(material, aiTextureType_AMBIENT_OCCLUSION);
	result.emissiveTexture = GetTextureReference(material, aiTextureType_EMISSIVE);
	result.metallicTexture = GetTextureReference(material, aiTextureType_METALNESS);
	result.roughnessTexture = GetTextureReference(material, aiTextureType_DIFFUSE_ROUGHNESS);

	materials.emplace_back(std::move(result));

	return true;

}

TextureReference ModelComponent::GetTextureReference(const aiMaterial* material, aiTextureType type)
{

	TextureReference reference;

	if (material->GetTextureCount(type) == 0)
	{
		return reference;
	}

	aiString path;
	aiReturn isSuccess = AI_FAILURE;

	isSuccess = material->GetTexture(type, 0, &path);

	if (isSuccess != AI_SUCCESS)
	{
		return reference;
	}

	std::string texturePath = path.C_Str();

	if (texturePath.empty() == true)
	{
		return reference;
	}

	//EmbeddedTexture
	//Assimpでは *0 や *1 のように表現される
	if (texturePath[0] == '*')
	{

		try
		{

			reference.embeddedTextureIndex = static_cast<int32_t>(std::stoul(texturePath.substr(1)));

		}
		catch (const std::exception&)
		{
			return TextureReference{};
		}

	}
	else
	{

		//外部ファイルの場合
		reference.path = texturePath;

	}

	return reference;

}

DirectX::XMFLOAT4X4 ModelComponent::ConvertMatrix(const aiMatrix4x4& matrix) const
{

	DirectX::XMFLOAT4X4 result;

	result._11 = matrix.a1;
	result._12 = matrix.a2;
	result._13 = matrix.a3;
	result._14 = matrix.a4;

	result._21 = matrix.b1;
	result._22 = matrix.b2;
	result._23 = matrix.b3;
	result._24 = matrix.b4;

	result._31 = matrix.c1;
	result._32 = matrix.c2;
	result._33 = matrix.c3;
	result._34 = matrix.c4;

	result._41 = matrix.d1;
	result._42 = matrix.d2;
	result._43 = matrix.d3;
	result._44 = matrix.d4;

	return result;
	
}