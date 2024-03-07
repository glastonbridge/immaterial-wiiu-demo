cmake_minimum_required(VERSION 3.5)
project(wiiu_demo C CXX)

# If adding more third party libs, you'll want to add them here
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lodepng
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/minivorbis
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rocket
)

# Add sources here
add_executable(wiiu_demo
   main.cpp
   renderer/Renderer.cpp
   renderer/RenderObject.cpp
   renderer/RenderMaterial.cpp
   renderer/RenderTexture.cpp
   graphics/SceneObject.cpp
   util/memory.cpp
   sound/Music.cpp
   sync/Sync.cpp
   third_party/lodepng/lodepng.cpp
   third_party/minivorbis/minivorbis.c
   third_party/rocket/device.c
   third_party/rocket/tcp.c
   third_party/rocket/track.c
   )

# Sync IP
set(SYNC_IP "127.0.0.1" CACHE STRING "IP of the computer running the synctool")
set(SYNC_PLAYER "true" CACHE BOOL "Set to false to connect to rocket synctool instead of using tracks")
target_compile_definitions(wiiu_demo PRIVATE SYNC_IP="${SYNC_IP}")
if(SYNC_PLAYER)
    target_compile_definitions(wiiu_demo PRIVATE SYNC_PLAYER=1)
endif()

# Basic WUT makefile
wut_create_rpx(wiiu_demo)

# Installation setup - add asset folders here
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
