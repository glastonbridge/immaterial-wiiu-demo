cmake_minimum_required(VERSION 3.5)
project(wiiu_demo CXX)

add_executable(wiiu_demo
   main.cpp
   renderer/Renderer.cpp
   renderer/RenderObject.cpp
   graphics/Material.cpp
   graphics/SceneObject.cpp)

wut_create_rpx(wiiu_demo)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/wiiu_demo.rpx"
        DESTINATION "${CMAKE_INSTALL_PREFIX}")
