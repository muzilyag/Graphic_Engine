#include "engine/renderer/Shader.h"
#include "engine/platform/GLLoader.h"
#include <fstream>
#include <sstream>

Shader::Shader(const char* vPath, const char* fPath)
    : logger("Shader"), vertexPath(vPath ? vPath : ""), fragmentPath(fPath ? fPath : "")
{
    ID = compileProgram(vertexPath, fragmentPath);
}

Shader::~Shader()
{
    if (ID != 0)
    {
        glDeleteProgram(ID);
        ID = 0;
    }
}

unsigned int Shader::compileProgram(const std::string& vPath, const std::string& fPath)
{
    std::string vCode = readFile(vPath.c_str());
    std::string fCode = readFile(fPath.c_str());
    if (vCode.empty() || fCode.empty())
    {
        logger.error("Shader source is empty, skipping compilation.");
        return 0;
    }

    const char* vPtr = vCode.c_str();
    const char* fPtr = fCode.c_str();

    unsigned int v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vPtr, NULL);
    glCompileShader(v);
    bool vOk = checkCompileErrors(v, "VERTEX");

    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fPtr, NULL);
    glCompileShader(f);
    bool fOk = checkCompileErrors(f, "FRAGMENT");

    if (!vOk || !fOk)
    {
        glDeleteShader(v);
        glDeleteShader(f);
        return 0;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);
    bool linkOk = checkCompileErrors(program, "PROGRAM");

    glDeleteShader(v);
    glDeleteShader(f);

    if (!linkOk)
    {
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

bool Shader::reload()
{
    unsigned int newProgram = compileProgram(vertexPath, fragmentPath);
    if (newProgram == 0)
    {
        logger.warn("Shader reload failed, keeping previous program: " + vertexPath);
        return false;
    }

    if (ID != 0)
    {
        glDeleteProgram(ID);
    }

    ID = newProgram;
    uniformCache.clear();
    logger.info("Reloaded shader: " + vertexPath + ", " + fragmentPath);
    return true;
}

void Shader::use() 
{ 
    glUseProgram(ID); 
}

bool Shader::hasUniform(const std::string& name)
{
    return getUniformLocation(name, false) != -1;
}

int Shader::getUniformLocation(const std::string& name, bool warnIfMissing)
{
    auto it = uniformCache.find(name);
    if (it != uniformCache.end())
    {
        return it->second;
    }

    int location = glGetUniformLocation(ID, name.c_str());
    if (location == -1 && warnIfMissing)
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

void Shader::setVec2(const std::string& name, float x, float y)
{
    int loc = getUniformLocation(name);
    if (loc != -1)
    {
        glUniform2f(loc, x, y);
    }
}

bool Shader::checkCompileErrors(unsigned int shader, const std::string& type) 
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
            return false;
        }
    } 
    else 
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) 
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            logger.error("ERROR::PROGRAM_LINKING_ERROR of type: " + type + "; " + infoLog);
            return false;
        }
    }
    return true;
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
    if (hasUniform(name + ".albedo"))
    {
        setVec3(name + ".albedo", mater.albedo);
    }
    if (hasUniform(name + ".metallic"))
    {
        setFloat(name + ".metallic", mater.metallic);
    }
    if (hasUniform(name + ".roughness"))
    {
        setFloat(name + ".roughness", mater.roughness);
    }
    if (hasUniform(name + ".ao"))
    {
        setFloat(name + ".ao", mater.ao);
    }
    if (hasUniform(name + ".emissive"))
    {
        setVec3(name + ".emissive", mater.emissive);
    }
    if (hasUniform(name + ".opacity"))
    {
        setFloat(name + ".opacity", mater.opacity);
    }
    if (hasUniform(name + ".alphaCutoff"))
    {
        setFloat(name + ".alphaCutoff", mater.alphaCutoff);
    }
    if (hasUniform(name + ".transmission"))
    {
        setFloat(name + ".transmission", mater.transmission);
    }
    if (hasUniform(name + ".ior"))
    {
        setFloat(name + ".ior", mater.ior);
    }
    if (hasUniform(name + ".refractionStrength"))
    {
        setFloat(name + ".refractionStrength", mater.refractionStrength);
    }
    if (hasUniform(name + ".chromaticAberration"))
    {
        setFloat(name + ".chromaticAberration", mater.chromaticAberration);
    }
}