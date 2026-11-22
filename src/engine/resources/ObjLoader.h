#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <memory>
#include <string>
#include "engine/renderer/Mesh.h"

class ObjLoader
{
public:
    static std::unique_ptr<Mesh> load(const std::string& path);
};
#endif