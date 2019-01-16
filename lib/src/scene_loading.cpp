#include <glmlv/scene_loading.hpp>

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <stack>

#ifdef GLMLV_USE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/ProgressHandler.hpp>
#endif

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace glmlv
{

#ifdef GLMLV_USE_ASSIMP
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

void loadAssimpScene(const fs::path & objPath, const fs::path & mtlBaseDir, SceneData & data, bool loadTextures)
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
					const auto index = uint32_t(indexOffset + face.mIndices[j]);
					data.indexBuffer.emplace_back(index);
					data.bboxMin = glm::min(data.bboxMin, data.vertexBuffer[index].position);
					data.bboxMax = glm::max(data.bboxMax, data.vertexBuffer[index].position);
				}
			}

			data.materialIDPerShape.emplace_back(mesh->mMaterialIndex >= 0 ? int(materialIdOffset + mesh->mMaterialIndex) : -1);

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
				textureIds[keyVal.first] = int32_t(data.textures.size());
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
			for (auto i = 0u; i < material->mNumProperties; ++i)
			{
				std::clog << "Property " << material->mProperties[i]->mKey.C_Str() << ", " << material->mProperties[i]->mType << ", " << material->mProperties[i]->mIndex << std::endl;
				if (material->mProperties[i]->mType == aiPTI_String)
				{
					aiString x;
					material->Get(material->mProperties[i]->mKey.C_Str(), material->mProperties[i]->mSemantic, material->mProperties[i]->mIndex, x);

					std::clog << material->mProperties[i]->mKey.C_Str() << ", " << "Value = " << x.C_Str() << std::endl;
				}
			}

			for (auto i = 0u; i < material->GetTextureCount(aiTextureType_UNKNOWN); ++i)
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
#endif

struct TinyObjLoaderIndexHash
{
    size_t operator()(const tinyobj::index_t & idx) const
    {
        const size_t h1 = std::hash<int>()(idx.vertex_index);
        const size_t h2 = std::hash<int>()(idx.normal_index);
        const size_t h3 = std::hash<int>()(idx.texcoord_index);

        size_t seed = h1;
        seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return seed;
    }
};

struct TinyObjLoaderEqualTo
{
    bool operator ()(const tinyobj::index_t & lhs, const tinyobj::index_t & rhs) const
    {
        return lhs.vertex_index == rhs.vertex_index && lhs.normal_index == rhs.normal_index && lhs.texcoord_index == rhs.texcoord_index;
    }
};

// Load an obj model with tinyobjloader
// Obj models might use different set of indices per vertex. The default rendering mechanism of OpenGL does not support this feature to this functions duplicate attributes with different indices.
void loadTinyObjScene(const fs::path & objPath, const fs::path & mtlBaseDir, SceneData & data, bool loadTextures)
{
    // Load obj
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attribs;

    std::string err;
    bool ret = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, objPath.string().c_str(), (mtlBaseDir.string() + "/").c_str());

    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }

    if (!ret) {
        throw std::runtime_error(err);
    }

    data.shapeCount += shapes.size();

    std::unordered_map<tinyobj::index_t, uint32_t, TinyObjLoaderIndexHash, TinyObjLoaderEqualTo> indexMap;

    std::unordered_set<std::string> texturePaths;

    const auto materialIdOffset = data.materials.size();
    for (const auto & shape : shapes)
    {
        const auto & mesh = shape.mesh;
        for (const auto & idx : mesh.indices)
        {
            const auto it = indexMap.find(idx);
            if (it == end(indexMap))
            {
                // Put the vertex in the vertex buffer and record a new index if not found
                float vx = attribs.vertices[3 * idx.vertex_index + 0];
                float vy = attribs.vertices[3 * idx.vertex_index + 1];
                float vz = attribs.vertices[3 * idx.vertex_index + 2];
                float nx = attribs.normals[3 * idx.normal_index + 0];
                float ny = attribs.normals[3 * idx.normal_index + 1];
                float nz = attribs.normals[3 * idx.normal_index + 2];
                float tx = attribs.texcoords[2 * idx.texcoord_index + 0];
                float ty = attribs.texcoords[2 * idx.texcoord_index + 1];

                uint32_t newIndex = data.vertexBuffer.size();
                data.vertexBuffer.emplace_back(glm::vec3(vx, vy, vz), glm::vec3(nx, ny, nz), glm::vec2(tx, ty));
                data.bboxMin = glm::min(data.bboxMin, data.vertexBuffer.back().position);
                data.bboxMax = glm::max(data.bboxMax, data.vertexBuffer.back().position);

                indexMap[idx] = newIndex;
                data.indexBuffer.emplace_back(newIndex);
            }
            else
                data.indexBuffer.emplace_back((*it).second);
        }
        data.indexCountPerShape.emplace_back(mesh.indices.size());

        const int32_t localMaterialID = mesh.material_ids.empty() ? -1 : mesh.material_ids[0];
        const int32_t materialID = localMaterialID >= 0 ? materialIdOffset + localMaterialID : -1;

        data.materialIDPerShape.emplace_back(materialID);
        data.localToWorldMatrixPerShape.emplace_back(glm::mat4(1.f));

        // Only load textures that are used
        if (localMaterialID >= 0)
        {
            const auto & material = materials[localMaterialID];
            texturePaths.emplace(material.ambient_texname);
            texturePaths.emplace(material.diffuse_texname);
            texturePaths.emplace(material.specular_texname);
            texturePaths.emplace(material.specular_highlight_texname);
        }
    }

    std::unordered_map<std::string, int32_t> textureIdMap;

    if (loadTextures)
    {
        const auto textureIdOffset = data.textures.size();
        for (const auto & texturePath : texturePaths)
        {
            if (!texturePath.empty())
            {
                auto newTexturePath = texturePath;
                std::replace(begin(newTexturePath), end(newTexturePath), '\\', '/');
                const auto completePath = mtlBaseDir / newTexturePath;
                if (fs::exists(completePath))
                {
                    std::clog << "Loading image " << completePath << std::endl;
                    data.textures.emplace_back(readImage(completePath));
                    data.textures.back().flipY();

                    const auto localTexId = textureIdMap.size();
                    textureIdMap[texturePath] = textureIdOffset + localTexId;
                }
                else
                {
                    std::clog << "'Warning: image " << completePath << " not found" << std::endl;
                }
            }
        }
    }

    for (const auto & material : materials)
    {
        data.materials.emplace_back(); // Add new material
        auto & newMaterial = data.materials.back();

        newMaterial.Ka = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
        newMaterial.Kd = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        newMaterial.Ks = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
        newMaterial.shininess = material.shininess;

        if (!material.ambient_texname.empty()) {
            const auto it = textureIdMap.find(material.ambient_texname);
            newMaterial.KaTextureId = it != end(textureIdMap) ? (*it).second : -1;
        }
        if (!material.diffuse_texname.empty()) {
            const auto it = textureIdMap.find(material.diffuse_texname);
            newMaterial.KdTextureId = it != end(textureIdMap) ? (*it).second : -1;
        }
        if (!material.specular_texname.empty()) {
            const auto it = textureIdMap.find(material.specular_texname);
            newMaterial.KsTextureId = it != end(textureIdMap) ? (*it).second : -1;
        }
        if (!material.specular_highlight_texname.empty()) {
            const auto it = textureIdMap.find(material.specular_highlight_texname);
            newMaterial.shininessTextureId = it != end(textureIdMap) ? (*it).second : -1;
        }
    }
}

}
