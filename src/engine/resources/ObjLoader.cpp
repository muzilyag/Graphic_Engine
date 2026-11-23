#include "engine/resources/ObjLoader.h"
#include "engine/utils/Logger.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace
{
    struct VertexKey
    {
        int v;
        int vt;
        int vn;

        bool operator==(const VertexKey& other) const
        {
            return v == other.v && vt == other.vt && vn == other.vn;
        }
    };

    void hashCombine(size_t& seed, size_t value)
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    struct VertexKeyHash
    {
        size_t operator()(const VertexKey& k) const
        {
            size_t seed = std::hash<int>{}(k.v);
            hashCombine(seed, std::hash<int>{}(k.vt));
            hashCombine(seed, std::hash<int>{}(k.vn));
            return seed;
        }
    };

    struct TexCoord
    {
        float u;
        float v;
    };

    int resolveIndex(int rawIndex, size_t count)
    {
        if (rawIndex > 0)
        {
            int idx = rawIndex - 1;
            return (idx >= 0 && static_cast<size_t>(idx) < count) ? idx : -1;
        }
        if (rawIndex < 0)
        {
            int idx = static_cast<int>(count) + rawIndex;
            return (idx >= 0 && static_cast<size_t>(idx) < count) ? idx : -1;
        }
        return -1;
    }

    VertexKey parseFaceToken(const std::string& token, size_t vCount, size_t vtCount, size_t vnCount)
    {
        VertexKey key{ -1, -1, -1 };

        size_t firstSlash = token.find('/');
        if (firstSlash == std::string::npos)
        {
            key.v = resolveIndex(std::stoi(token), vCount);
            return key;
        }

        key.v = resolveIndex(std::stoi(token.substr(0, firstSlash)), vCount);

        size_t secondSlash = token.find('/', firstSlash + 1);
        if (secondSlash == std::string::npos)
        {
            std::string vtStr = token.substr(firstSlash + 1);
            if (!vtStr.empty())
            {
                key.vt = resolveIndex(std::stoi(vtStr), vtCount);
            }
            return key;
        }

        std::string vtStr = token.substr(firstSlash + 1, secondSlash - firstSlash - 1);
        if (!vtStr.empty())
        {
            key.vt = resolveIndex(std::stoi(vtStr), vtCount);
        }

        std::string vnStr = token.substr(secondSlash + 1);
        if (!vnStr.empty())
        {
            key.vn = resolveIndex(std::stoi(vnStr), vnCount);
        }

        return key;
    }

    Vec3 fallbackUpIfZero(const Vec3& v)
    {
        if (v.x == 0.0f && v.y == 0.0f && v.z == 0.0f)
        {
            return { 0.0f, 1.0f, 0.0f };
        }
        return v;
    }
}

std::unique_ptr<Mesh> ObjLoader::load(const std::string& path)
{
    Logger logger("ObjLoader");

    std::ifstream file(path);
    if (!file.is_open())
    {
        logger.error("Failed to open OBJ file: " + path);
        return nullptr;
    }

    std::vector<Vec3> positions;
    std::vector<TexCoord> texCoords;
    std::vector<Vec3> normals;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::unordered_map<VertexKey, unsigned int, VertexKeyHash> uniqueVertices;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            Vec3 pos{};
            if (iss >> pos.x >> pos.y >> pos.z)
            {
                positions.push_back(pos);
            }
        }
        else if (prefix == "vt")
        {
            TexCoord uv{};
            if (iss >> uv.u >> uv.v)
            {
                texCoords.push_back(uv);
            }
        }
        else if (prefix == "vn")
        {
            Vec3 norm{};
            if (iss >> norm.x >> norm.y >> norm.z)
            {
                normals.push_back(fallbackUpIfZero(Vec3::normalize(norm)));
            }
        }
        else if (prefix == "f")
        {
            std::vector<VertexKey> faceKeys;
            std::string token;
            while (iss >> token)
            {
                try
                {
                    VertexKey key = parseFaceToken(token, positions.size(), texCoords.size(), normals.size());
                    if (key.v >= 0)
                    {
                        faceKeys.push_back(key);
                    }
                }
                catch (...)
                {
                    logger.warn("Skipping malformed face token in OBJ: " + path);
                }
            }

            if (faceKeys.size() < 3)
            {
                continue;
            }

            for (size_t i = 1; i + 1 < faceKeys.size(); ++i)
            {
                VertexKey tri[3] = { faceKeys[0], faceKeys[i], faceKeys[i + 1] };

                bool needsFlatNormal = (tri[0].vn < 0 || tri[1].vn < 0 || tri[2].vn < 0);
                Vec3 flatNormal{ 0.0f, 1.0f, 0.0f };
                if (needsFlatNormal)
                {
                    Vec3 e1 = positions[tri[1].v] - positions[tri[0].v];
                    Vec3 e2 = positions[tri[2].v] - positions[tri[0].v];
                    flatNormal = fallbackUpIfZero(Vec3::normalize(Vec3::cross(e1, e2)));
                }

                for (int corner = 0; corner < 3; ++corner)
                {
                    const VertexKey& key = tri[corner];

                    if (key.vn >= 0)
                    {
                        auto it = uniqueVertices.find(key);
                        if (it != uniqueVertices.end())
                        {
                            indices.push_back(it->second);
                            continue;
                        }
                    }

                    Vertex vert{};
                    vert.position = positions[key.v];
                    vert.normal = (key.vn >= 0) ? normals[key.vn] : flatNormal;
                    if (key.vt >= 0)
                    {
                        vert.u = texCoords[key.vt].u;
                        vert.v = texCoords[key.vt].v;
                    }
                    else
                    {
                        vert.u = 0.0f;
                        vert.v = 0.0f;
                    }

                    unsigned int newIndex = static_cast<unsigned int>(vertices.size());
                    vertices.push_back(vert);
                    indices.push_back(newIndex);

                    if (key.vn >= 0)
                    {
                        uniqueVertices[key] = newIndex;
                    }
                }
            }
        }
    }

    if (vertices.empty() || indices.empty())
    {
        logger.error("OBJ file contains no geometry: " + path);
        return nullptr;
    }

    logger.info("Loaded OBJ: " + path + " (Vertices: " + std::to_string(vertices.size()) + ", Indices: " + std::to_string(indices.size()) + ")");
    return std::make_unique<Mesh>(vertices, indices);
}