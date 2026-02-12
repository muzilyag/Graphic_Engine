#ifndef SHADER_H
#define SHADER_H
#include <string>
#include "Math3d.h"
#include "Logger.h"
#include "Material.h"

class Shader 
{
    Logger logger;
    unsigned int ID;

    std::string readFile(const char* path);
    void checkCompileErrors(unsigned int shader, std::string type);
public:
    Shader(const char* vPath, const char* fPath);
    void use();
    // Setters
    void setMat4(const char* name, const Mat4& mat);
    void setVec3(const char* name, const Vec3& vec);
    void setFloat(const char* name, float value);
    void setInt(const char* name, int value);
    void setMaterial(const std::string& name, const Material& mater);
};
#endif