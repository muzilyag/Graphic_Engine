cmake_minimum_required(VERSION 3.10)
project(Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENGINE_SOURCES
    "src/engine/core/Application.cpp"
    "src/engine/platform/Window.cpp"
    "src/engine/platform/GLLoader.cpp"
    "src/engine/math/Math3d.cpp"
    "src/engine/renderer/Mesh.cpp"
    "src/engine/renderer/Primitives.cpp"
    "src/engine/renderer/Renderer.cpp"
    "src/engine/renderer/Shader.cpp"
    "src/engine/renderer/Texture.cpp"
    "src/engine/resources/ImageLoader.cpp"
    "src/engine/resources/ObjLoader.cpp"
    "src/engine/resources/ResourceManager.cpp"
    "src/engine/scene/Camera.cpp"
    "src/engine/scene/Entity.cpp"
    "src/engine/scene/Scene.cpp"
    "src/engine/scene/Transform.cpp"
    "src/engine/utils/Logger.cpp"
)

add_library(EngineLib STATIC ${ENGINE_SOURCES})

target_include_directories(EngineLib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if(WIN32)
    target_compile_definitions(EngineLib PUBLIC
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    target_link_libraries(EngineLib PUBLIC
        opengl32
        gdi32
        user32
        kernel32
    )
endif()

add_executable(Sandbox
    "src/sandbox/Main.cpp"
    "src/sandbox/SandboxApp.cpp"
)

target_link_libraries(Sandbox PRIVATE EngineLib)

add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Sandbox>/assets"
)