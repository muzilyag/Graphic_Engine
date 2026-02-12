#ifndef MATERIAL_H
#define MATERIAL_H
#include "Math3d.h"

struct Material
{
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    float shininess;
};

struct PointLight 
{
    Vec3 position;
    Vec3 color;
    float constant;
    float linear;
    float quadratic;
};
#endif