#include "Shader.h"
#include "GLLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vPath, const char* fPath) : logger("Shader")
{
    std::string vCode = readFile(vPath);
    std::string fCode = readFile(fPath);
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
}

void Shader::use() 
{ 
    glUseProgram(ID); 
}

void Shader::setMat4(const char* name, const Mat4& mat) 
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, mat.m);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) 
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") 
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) 
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            // std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
            logger.error("ERROR::PROGRAM_LINKING_ERROR of type:" + type + "; " + infoLog);
        }
    } 
    else 
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) 
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            logger.error("ERROR::PROGRAM_LINKING_ERROR of type:" + type + "; " + infoLog);
            // std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
        }
    }
}

std::string Shader::readFile(const char* path) 
{
    std::ifstream f(path);
    std::stringstream s;
    s << f.rdbuf();
    return s.str();
}

void Shader::setVec3(const char* name, const Vec3& vec)
{
    glUniform3f(glGetUniformLocation(ID, name), vec.x, vec.y, vec.z);
}

void Shader::setFloat(const char* name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::setInt(const char* name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::setMaterial(const std::string& name, const Material& mater) 
{
    setVec3((name + ".ambient").c_str(), mater.ambient);
    setVec3((name + ".diffuse").c_str(), mater.diffuse);
    setVec3((name + ".specular").c_str(), mater.specular);
    setFloat((name + ".shininess").c_str(), mater.shininess);
}