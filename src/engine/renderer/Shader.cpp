#include "engine/renderer/Shader.h"
#include "engine/platform/GLLoader.h"
#include <fstream>
#include <sstream>

Shader::Shader(const char* vPath, const char* fPath) : logger("Shader")
{
    std::string vCode = readFile(vPath);
    std::string fCode = readFile(fPath);
    if (vCode.empty() || fCode.empty())
    {
        logger.error("Shader source is empty, skipping compilation.");
        return;
    }

    const char* vPtr = vCode.c_str();
    const char* fPtr = fCode.c_str();

    unsigned int v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vPtr, NULL);
    glCompileShader(v);
    checkCompileErrors(v, "VERTEX");

    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fPtr, NULL);
    glCompileShader(f);
    checkCompileErrors(f, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, v);
    glAttachShader(ID, f);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(v);
    glDeleteShader(f);
}

Shader::~Shader()
{
    if (ID != 0)
    {
        glDeleteProgram(ID);
        ID = 0;
    }
}

void Shader::use() 
{ 
    glUseProgram(ID); 
}

int Shader::getUniformLocation(const std::string& name)
{
    auto it = uniformCache.find(name);
    if (it != uniformCache.end())
    {
        return it->second;
    }

    int location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
    {
        logger.warn("Uniform not found or unused: " + name);
    }

    uniformCache[name] = location;
    return location;
}

void Shader::setMat4(const std::string& name, const Mat4& mat) 
{
    int loc = getUniformLocation(name);
    if (loc != -1)
    {
        glUniformMatrix4fv(loc, 1, GL_FALSE, mat.m);
    }
}

void Shader::checkCompileErrors(unsigned int shader, const std::string& type) 
{
    int success = 0;
    char infoLog[1024];
    if (type != "PROGRAM") 
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) 
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            logger.error("ERROR::SHADER_COMPILATION_ERROR of type: " + type + "; " + infoLog);
        }
    } 
    else 
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) 
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            logger.error("ERROR::PROGRAM_LINKING_ERROR of type: " + type + "; " + infoLog);
        }
    }
}

std::string Shader::readFile(const char* path) 
{
    std::ifstream f(path);
    if (!f.is_open())
    {
        logger.error("Failed to open shader file: " + std::string(path));
        return "";
    }
    std::stringstream s;
    s << f.rdbuf();
    return s.str();
}

void Shader::setVec3(const std::string& name, const Vec3& vec)
{
    int loc = getUniformLocation(name);
    if (loc != -1)
    {
        glUniform3f(loc, vec.x, vec.y, vec.z);
    }
}

void Shader::setFloat(const std::string& name, float value)
{
    int loc = getUniformLocation(name);
    if (loc != -1)
    {
        glUniform1f(loc, value);
    }
}

void Shader::setInt(const std::string& name, int value)
{
    int loc = getUniformLocation(name);
    if (loc != -1)
    {
        glUniform1i(loc, value);
    }
}

void Shader::setMaterial(const std::string& name, const Material& mater) 
{
    setVec3(name + ".ambient", mater.ambient);
    setVec3(name + ".diffuse", mater.diffuse);
    setVec3(name + ".specular", mater.specular);
    setFloat(name + ".shininess", mater.shininess);
}