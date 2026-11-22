#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <unordered_map>
#include "engine/math/Math3d.h"
#include "engine/utils/Logger.h"
#include "engine/renderer/Material.h"

class Shader 
{
    Logger logger;
    unsigned int ID = 0;
    std::unordered_map<std::string, int> uniformCache;

    std::string readFile(const char* path);
    void checkCompileErrors(unsigned int shader, const std::string& type);
    int getUniformLocation(const std::string& name);
public:
    Shader(const char* vPath, const char* fPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use();
    // Setters
    void setMat4(const std::string& name, const Mat4& mat);
    void setVec3(const std::string& name, const Vec3& vec);
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);
    void setMaterial(const std::string& name, const Material& mater);
};
#endif