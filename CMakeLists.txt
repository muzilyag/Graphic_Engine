cmake_minimum_required(VERSION 3.10)
project(Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    src
    src/engine/core
    src/engine/renderer
    src/engine/utils
    src/engine/math
)

set(SOURCES
    "src/Main.cpp"
    "src/engine/core/Application.cpp"
    "src/engine/core/Window.cpp"
    "src/engine/math/Math3d.cpp"
    "src/engine/renderer/Camera.cpp"
    "src/engine/renderer/GLLoader.cpp"
    "src/engine/renderer/Mesh.cpp"
    "src/engine/renderer/Shader.cpp"
    "src/engine/renderer/Texture.cpp"
    "src/engine/renderer/Transform.cpp"
    "src/engine/utils/Logger.cpp"
    )

add_executable(${PROJECT_NAME} ${SOURCES})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} 
        opengl32 
        gdi32 
        user32 
        kernel32
    )
endif()