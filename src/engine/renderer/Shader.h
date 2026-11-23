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
    std::string vertexPath;
    std::string fragmentPath;
    std::unordered_map<std::string, int> uniformCache;

    std::string readFile(const char* path);
    bool checkCompileErrors(unsigned int shader, const std::string& type);
    unsigned int compileProgram(const std::string& vPath, const std::string& fPath);
    int getUniformLocation(const std::string& name, bool warnIfMissing = true);
    void trySetVec3(const std::string& name, const Vec3& vec);
    void trySetFloat(const std::string& name, float value);
public:
    Shader(const char* vPath, const char* fPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    bool reload();
    void use();
    bool isValid() const { return ID != 0; }
    bool hasUniform(const std::string& name);
    // Setters
    void setMat4(const std::string& name, const Mat4& mat);
    void setVec2(const std::string& name, float x, float y);
    void setVec3(const std::string& name, const Vec3& vec);
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);
    void setMaterial(const std::string& name, const Material& mater);
};
#endif