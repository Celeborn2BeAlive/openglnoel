#include <glmlv/simple_geometry.hpp>
#include <glm/gtc/constants.hpp>

namespace glmlv
{

SimpleGeometry makeTriangle()
{
    std::vector<Vertex3f3f2f> vertexBuffer =
    {
        { glm::vec3(-0.5, -0.5, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, 0), glm::vec3(0, 0, 1), glm::vec2(1, 0) },
        { glm::vec3(0., 0.5, 0), glm::vec3(0, 0, 1), glm::vec2(0.5, 1) }
    };

    std::vector<uint32_t> indexBuffer =
    {
        0, 1, 2
    };

    return{ vertexBuffer, indexBuffer };
}

SimpleGeometry makeCube()
{
    std::vector<Vertex3f3f2f> vertexBuffer =
    {
        // Bottom side
        { glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec2(0, 1) },
        { glm::vec3(0.5, -0.5, 0.5), glm::vec3(0, -1, 0), glm::vec2(1, 1) },
        { glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, -1, 0), glm::vec2(1, 0) },
        // Right side
        { glm::vec3(0.5, -0.5, 0.5), glm::vec3(1, 0, 0), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(0, 1) },
        { glm::vec3(0.5, 0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(1, 1) },
        { glm::vec3(0.5, 0.5, 0.5), glm::vec3(1, 0, 0), glm::vec2(1, 0) },
        // Back side
        { glm::vec3(0.5, -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(0, 0) },
        { glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(0, 1) },
        { glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(1, 1) },
        { glm::vec3(0.5, 0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(1, 0) },
        // Left side
        { glm::vec3(-0.5, -0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 0) },
        { glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 1) },
        { glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 1) },
        { glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 0) },
        // Front side
        { glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(0, 1) },
        { glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(1, 1) },
        { glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(1, 0) },
        // Top side
        { glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 1, 0), glm::vec2(0, 0) },
        { glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 1, 0), glm::vec2(0, 1) },
        { glm::vec3(0.5, 0.5, -0.5), glm::vec3(0, 1, 0), glm::vec2(1, 1) },
        { glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 1, 0), glm::vec2(1, 0) }
    };

    std::vector<uint32_t> indexBuffer =
    {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
        8, 9, 10,
        8, 10, 11,
        12, 13, 14,
        12, 14, 15,
        16, 17, 18,
        16, 18, 19,
        20, 21, 22,
        20, 22, 23
    };

    return{ vertexBuffer, indexBuffer };
}

SimpleGeometry makeSphere(uint32_t subdivLongitude)
{
    const auto discLong = subdivLongitude;
    const auto discLat = 2 * discLong;

    float rcpLat = 1.f / discLat, rcpLong = 1.f / discLong;
    float dPhi = glm::pi<float>() * 2.f * rcpLat, dTheta = glm::pi<float>() * rcpLong;

    std::vector<Vertex3f3f2f> vertexBuffer;

    for (uint32_t j = 0; j <= discLong; ++j)
    {
        float cosTheta = cos(-glm::half_pi<float>() + j * dTheta);
        float sinTheta = sin(-glm::half_pi<float>() + j * dTheta);

        for (uint32_t i = 0; i <= discLat; ++i) {
            glm::vec3 coords;

            coords.x = sin(i * dPhi) * cosTheta;
            coords.y = sinTheta;
            coords.z = cos(i * dPhi) * cosTheta;

            vertexBuffer.emplace_back(coords, coords, glm::vec2(i * rcpLat, j * rcpLong));
        }
    }

    std::vector<uint32_t> indexBuffer;

    for (uint32_t j = 0; j < discLong; ++j)
    {
        uint32_t offset = j * (discLat + 1);
        for (uint32_t i = 0; i < discLat; ++i)
        {
            indexBuffer.push_back(offset + i);
            indexBuffer.push_back(offset + (i + 1));
            indexBuffer.push_back(offset + discLat + 1 + (i + 1));

            indexBuffer.push_back(offset + i);
            indexBuffer.push_back(offset + discLat + 1 + (i + 1));
            indexBuffer.push_back(offset + i + discLat + 1);
        }
    }

    return{ vertexBuffer, indexBuffer };
}

}