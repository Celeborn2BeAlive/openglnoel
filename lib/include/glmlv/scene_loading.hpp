#pragma once

#include <glmlv/simple_geometry.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <glmlv/filesystem.hpp>
#include <glm/vec3.hpp>

namespace glmlv
{
    struct SceneData
    {
        struct PhongMaterial
        {
			std::string name;

            glm::vec3 Ka = glm::vec3(0); // Ambient multiplier
            glm::vec3 Kd = glm::vec3(0); // Diffuse multiplier
            glm::vec3 Ks = glm::vec3(0); // Glossy multiplier
            float shininess = 0.f; // Glossy exponent

            // Indices in the textures vector:
            int32_t KaTextureId = -1;
            int32_t KdTextureId = -1;
            int32_t KsTextureId = -1;
            int32_t shininessTextureId = -1;
        };

        glm::vec3 bboxMin = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 bboxMax = glm::vec3(std::numeric_limits<float>::lowest());

        std::vector<Vertex3f3f2f> vertexBuffer;
        std::vector<uint32_t> indexBuffer;

		size_t shapeCount = 0;
        std::vector<uint32_t> indexCountPerShape;
		std::vector<glm::mat4> localToWorldMatrixPerShape;
        std::vector<int32_t> materialIDPerShape;

        std::vector<PhongMaterial> materials;
        std::vector<Image2DRGBA> textures;
    };

    void loadScene(const fs::path & objPath, const fs::path & mtlBaseDir, SceneData & data, bool loadTextures = true);

    inline void loadScene(const fs::path & objPath, SceneData & data, bool loadTextures = true)
    {
        return loadScene(objPath, objPath.parent_path(), data, loadTextures);
    }
}