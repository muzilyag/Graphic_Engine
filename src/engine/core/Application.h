#ifndef APPLICATION_H
#define APPLICATION_H
#include <memory>
#include "Window.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"
#include "Transform.h"

class Application 
{
    Camera camera;
    Window window;

    std::unique_ptr<Shader> cubeShader;
    std::unique_ptr<Texture> cubeTexture;
    std::unique_ptr<Texture> specularMap;
    std::unique_ptr<Texture> normalMap;
    std::unique_ptr<Mesh> cubeMesh;

    Vec3 positions[5] = {
        { 0.0f,  0.0f,  0.0f},
        { 2.0f,  5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        { 2.4f, -0.4f, -3.5f}
    };

    Vec3 lightPositions[4] = {
        { 0.7f,  0.2f,  2.0f},
        { 2.3f, -3.3f, -4.0f},
        {-4.0f,  2.0f, -12.0f},
        { 0.0f,  0.0f, -3.0f}
    };

    Vec3 lightColors[4] = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 1.0f} 
    };

    float runTime;
    
    void update(float dt);
    void render();
public:
    Application();
    ~Application() = default;
    void run();
};
#endif