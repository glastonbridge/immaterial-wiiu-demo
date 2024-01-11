cmake_minimum_required(VERSION 3.5)
project(wiiu_demo C CXX)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lodepng
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/minivorbis
)

add_executable(wiiu_demo
   main.cpp
   renderer/Renderer.cpp
   renderer/RenderObject.cpp
   graphics/Material.cpp
   graphics/SceneObject.cpp
   math/projection.cpp
   util/memory.cpp
   sound/Music.cpp
   third_party/lodepng/lodepng.cpp
   third_party/minivorbis/minivorbis.c
   )

wut_create_rpx(wiiu_demo)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/wiiu_demo.rpx"
        DESTINATION "${CMAKE_INSTALL_PREFIX}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
         DESTINATION "${CMAKE_INSTALL_PREFIX}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/assets"
         DESTINATION "${CMAKE_INSTALL_PREFIX}")

# Emulator setup (make test)
set(CEMU_EXECUTABLE "cemu" CACHE STRING "Path to the emulator executable")
set(SDCARD_DIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "SD card directory (emulator side, defaults to install dir)")
add_custom_target(test
    COMMAND ${CMAKE_COMMAND} --build . --target install
    COMMAND ${CEMU_EXECUTABLE} -g "${SDCARD_DIR}/${PROJECT_NAME}.rpx"
)
