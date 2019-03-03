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

		// Points min et max de la bounding box englobant la scene
        glm::vec3 bboxMin = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 bboxMax = glm::vec3(std::numeric_limits<float>::lowest());

        std::vector<Vertex3f3f2f> vertexBuffer; // Tableau de sommets
        std::vector<uint32_t> indexBuffer; // Tableau d'index de sommets

		size_t shapeCount = 0; // Nombre d'objets à dessiner
        std::vector<uint32_t> indexCountPerShape; // Nomber d'index de sommets pour chaque objet
		std::vector<glm::mat4> localToWorldMatrixPerShape; // Matrice localToWorld de chaque objet
        std::vector<int32_t> materialIDPerShape; // Index du materiau de chaque objet (-1 si pas de materiaux)

        std::vector<PhongMaterial> materials; // Tableau des materiaux
        std::vector<Image2DRGBA> textures; // Tableau des textures référencés par les materiaux
    };

#ifdef GLMLV_USE_ASSIMP
    void loadAssimpScene(const fs::path & path, const fs::path & mtlBaseDir, SceneData & data, bool loadTextures = true);

    inline void loadAssimpScene(const fs::path & path, SceneData & data, bool loadTextures = true)
    {
        return loadAssimpScene(path, path.parent_path(), data, loadTextures);
    }
#endif

    void loadTinyObjScene(const fs::path & path, const fs::path & mtlBaseDir, SceneData & data, bool loadTextures = true);

    inline void loadTinyObjScene(const fs::path & path, SceneData & data, bool loadTextures = true)
    {
        return loadTinyObjScene(path, path.parent_path(), data, loadTextures);
    }

    inline void loadObjScene(const fs::path & path, const fs::path & mtlBaseDir, SceneData & data, bool loadTextures = true)
    {
#ifdef GLMLV_USE_ASSIMP
        return loadAssimpScene(path, mtlBaseDir, data, loadTextures);
#else
        return loadTinyObjScene(path, mtlBaseDir, data, loadTextures);
#endif
    }

    inline void loadObjScene(const fs::path & path, SceneData & data, bool loadTextures = true)
    {
        return loadObjScene(path, path.parent_path(), data, loadTextures);
    }
}