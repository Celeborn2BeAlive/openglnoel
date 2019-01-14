#include <glmlv/scene_loading.hpp>

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <stack>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/ProgressHandler.hpp>

namespace glmlv
{

glm::mat4 aiMatrixToGlmMatrix(const aiMatrix4x4 & mat)
{
	auto copy = mat;
	copy.Transpose();
	return glm::mat4(
		copy.a1, copy.a2, copy.a3, copy.a4, 
		copy.b1, copy.b2, copy.b3, copy.b4,
		copy.c1, copy.c2, copy.c3, copy.c4, 
		copy.d1, copy.d2, copy.d3, copy.d4
	);
}

void loadScene(const fs::path & objPath, const fs::path & mtlBaseDir, SceneData & data, bool loadTextures)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(objPath.string().c_str(), aiProcess_Triangulate);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		throw std::runtime_error(importer.GetErrorString());
	}

	std::stack<std::pair<aiNode*, aiMatrix4x4>> nodes;
	nodes.push(std::make_pair(scene->mRootNode, scene->mRootNode->mTransformation));

	std::unordered_map<std::string, int32_t> textureIds;

	const auto materialIdOffset = data.materials.size();
	
	while (!nodes.empty())
	{
		const auto node = nodes.top().first;
		const auto localToWorldMatrix = nodes.top().second;
		nodes.pop();

		data.shapeCount += node->mNumMeshes;

		for (auto meshIdx = 0u; meshIdx < node->mNumMeshes; ++meshIdx)
		{
			aiMesh * mesh = scene->mMeshes[node->mMeshes[meshIdx]];

			const auto indexOffset = data.vertexBuffer.size();

			data.vertexBuffer.reserve(data.vertexBuffer.size() + mesh->mNumVertices);
			for (auto vertexIdx = 0u; vertexIdx < mesh->mNumVertices; ++vertexIdx)
			{
				const float vx = mesh->HasPositions() ? mesh->mVertices[vertexIdx].x : 0.f;
				const float vy = mesh->HasPositions() ? mesh->mVertices[vertexIdx].y : 0.f;
				const float vz = mesh->HasPositions() ? mesh->mVertices[vertexIdx].z : 0.f;
				const float nx = mesh->HasNormals() ? mesh->mNormals[vertexIdx].x : 0.f;
				const float ny = mesh->HasNormals() ? mesh->mNormals[vertexIdx].y : 0.f;
				const float nz = mesh->HasNormals() ? mesh->mNormals[vertexIdx].z : 0.f;

				const float tx = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][vertexIdx].x : 0.f;
				const float ty = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][vertexIdx].y : 0.f;

				data.vertexBuffer.emplace_back(glm::vec3(vx, vy, vz), glm::vec3(nx, ny, nz), glm::vec2(tx, ty));
			}

			const auto indexCount = mesh->mNumFaces * 3;
			data.indexCountPerShape.emplace_back(indexCount);
			data.localToWorldMatrixPerShape.emplace_back(aiMatrixToGlmMatrix(localToWorldMatrix));
			data.indexBuffer.reserve(data.indexBuffer.size() + indexCount);
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				assert(face.mNumIndices == 3);
				for (unsigned int j = 0; j < face.mNumIndices; j++) 
				{
					const auto index = indexOffset + face.mIndices[j];
					data.indexBuffer.emplace_back(index);
					data.bboxMin = glm::min(data.bboxMin, data.vertexBuffer[index].position);
					data.bboxMax = glm::max(data.bboxMax, data.vertexBuffer[index].position);
				}
			}

			data.materialIDPerShape.emplace_back(mesh->mMaterialIndex >= 0 ? materialIdOffset + mesh->mMaterialIndex : -1);

			// Store texture paths to load them later
			if (loadTextures && mesh->mMaterialIndex >= 0)
			{
				aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];

				aiString path;
				if (AI_SUCCESS == material->GetTexture(aiTextureType_AMBIENT, 0, &path,
					nullptr, nullptr, nullptr, nullptr, nullptr)) {
					textureIds[path.data] = -1;
				}

				if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &path,
					nullptr, nullptr, nullptr, nullptr, nullptr)) {
					textureIds[path.data] = -1;
				}

				if (AI_SUCCESS == material->GetTexture(aiTextureType_SPECULAR, 0, &path,
					nullptr, nullptr, nullptr, nullptr, nullptr)) {
					textureIds[path.data] = -1;
				}

				if (AI_SUCCESS == material->GetTexture(aiTextureType_SHININESS, 0, &path,
					nullptr, nullptr, nullptr, nullptr, nullptr)) {
					textureIds[path.data] = -1;
				}
			}
		}

		for (auto childIdx = 0u; childIdx < node->mNumChildren; ++childIdx) {
			nodes.push(std::make_pair(node->mChildren[childIdx], node->mChildren[childIdx]->mTransformation * localToWorldMatrix));
		}
	}

	if (loadTextures)
	{
		for (const auto & keyVal : textureIds)
		{
			const auto completePath = mtlBaseDir / keyVal.first;
			if (fs::exists(completePath))
			{
				std::clog << "Loading image " << completePath << std::endl;
				textureIds[keyVal.first] = data.textures.size();
				data.textures.emplace_back(readImage(completePath));
				data.textures.back().flipY();
			}
			else
			{
				std::clog << "'Warning: image " << completePath << " not found" << std::endl;
			}
		}
	}

	// Materials
	data.materials.reserve(data.materials.size() + scene->mNumMaterials);
	for (auto materialIdx = 0u; materialIdx < scene->mNumMaterials; ++materialIdx)
	{
		aiMaterial * material = scene->mMaterials[materialIdx];

		data.materials.emplace_back(); // Add new material
		auto & newMaterial = data.materials.back();

		aiColor3D color;

		aiString ainame;
		material->Get(AI_MATKEY_NAME, ainame);
		newMaterial.name = ainame.C_Str();

		if (newMaterial.name == "Helmet")
		{
			std::clog << "Material " << newMaterial.name << std::endl;
			std::clog << material->mNumProperties << std::endl;
			for (auto i = 0; i < material->mNumProperties; ++i)
			{
				std::clog << "Property " << material->mProperties[i]->mKey.C_Str() << ", " << material->mProperties[i]->mType << ", " << material->mProperties[i]->mIndex << std::endl;
				if (material->mProperties[i]->mType == aiPTI_String)
				{
					aiString x;
					material->Get(material->mProperties[i]->mKey.C_Str(), material->mProperties[i]->mSemantic, material->mProperties[i]->mIndex, x);

					std::clog << material->mProperties[i]->mKey.C_Str() << ", " << "Value = " << x.C_Str() << std::endl;
				}
			}

			for (auto i = 0; i < material->GetTextureCount(aiTextureType_UNKNOWN); ++i)
			{
				aiString path;
				if (AI_SUCCESS == material->GetTexture(aiTextureType_UNKNOWN, i, &path,
					nullptr, nullptr, nullptr, nullptr, nullptr)) {
					std::cerr << "texture " << i << path.C_Str() << std::endl;
				}
			}
		}


		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
			newMaterial.Ka = glm::vec3(color.r, color.g, color.b);
		}

		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
			newMaterial.Kd = glm::vec3(color.r, color.g, color.b);
		}

		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
			newMaterial.Ks = glm::vec3(color.r, color.g, color.b);
		}

		material->Get(AI_MATKEY_SHININESS, newMaterial.shininess);

		if (loadTextures)
		{
			aiString path;
			if (AI_SUCCESS == material->GetTexture(aiTextureType_AMBIENT, 0, &path,
				nullptr, nullptr, nullptr, nullptr, nullptr)) {
				newMaterial.KaTextureId = textureIds[path.data];
			}

			if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &path,
				nullptr, nullptr, nullptr, nullptr, nullptr)) {
				newMaterial.KdTextureId = textureIds[path.data];
			}

			if (AI_SUCCESS == material->GetTexture(aiTextureType_SPECULAR, 0, &path,
				nullptr, nullptr, nullptr, nullptr, nullptr)) {
				newMaterial.KsTextureId = textureIds[path.data];
			}

			if (AI_SUCCESS == material->GetTexture(aiTextureType_SHININESS, 0, &path,
				nullptr, nullptr, nullptr, nullptr, nullptr)) {
				newMaterial.shininessTextureId = textureIds[path.data];
			}
		}
	}
}

}
