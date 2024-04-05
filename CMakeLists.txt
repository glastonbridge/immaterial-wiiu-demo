cmake_minimum_required(VERSION 3.5)
project(wiiu_demo C CXX)

# WUHB metadata
set(WUHB_TITLE "Immaterial" CACHE STRING "Title of the WUHB package")
set(WUHB_LONGTITLE "Immaterial" CACHE STRING "Long title of the WUHB package")
set(WUHB_AUTHOR "Slipstream and SVatG" CACHE STRING "Author of the WUHB package")
set(WUHB_ICON "../icon.png" CACHE STRING "Icon of the WUHB package (128x128)")
# TODO:
# set(WUHB_TV_IMAGE "tv.png" CACHE STRING "TV image of the WUHB package (1280x720)")
# set(WUHB_GAMEPAD_IMAGE "gamepad.png" CACHE STRING "Gamepad image of the WUHB package (854x480)")

# If adding more third party libs, you'll want to add them here
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lodepng
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/minivorbis
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rocket
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ufbx
)

# Add sources here
add_executable(wiiu_demo
    main.cpp
    renderer/Renderer.cpp
    renderer/RenderObject.cpp
    renderer/RenderMaterial.cpp
    renderer/RenderTexture.cpp
    renderer/RenderBuffer.cpp
    graphics/SceneObject.cpp
    graphics/ObjectFactory.cpp
    util/memory.cpp
    sound/Music.cpp
    sync/Sync.cpp
    third_party/lodepng/lodepng.cpp
    third_party/minivorbis/minivorbis.c
    third_party/rocket/device.c
    third_party/rocket/tcp.c
    third_party/rocket/track.c
    third_party/ufbx/ufbx.c
    graphics/LoadUFBX.cpp
    renderer/ShaderManager.cpp
    scenes/SceneAssets.cpp
    scenes/Scenes.cpp
    scenes/Spline.cpp
    scenes/RealScene.cpp
    scenes/EepyScene.cpp
    util/ourmalloc.cpp
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
install(DIRECTORY "${PROJECT_SOURCE_DIR}/sync_tracks"
         DESTINATION "${CMAKE_INSTALL_PREFIX}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/shader_cache"
         DESTINATION "${CMAKE_INSTALL_PREFIX}")

# Emulator setup (make test)
set(CEMU_EXECUTABLE "cemu" CACHE STRING "Path to the emulator executable")
set(SDCARD_DIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "SD card directory (emulator side, defaults to install dir)")
add_custom_target(test
    COMMAND ${CMAKE_COMMAND} --build . --target install
    COMMAND ${CEMU_EXECUTABLE} -g "${SDCARD_DIR}/${PROJECT_NAME}.rpx"
)

# Wii U wiiload target (doesn't copy the assets, do that yourself)
# until such time as we add ftp copy support
set(WIILOAD_IP "192.168.1.254" CACHE STRING "IP of the Wii U for wiiload")
add_custom_target(wiiload
    COMMAND ${CMAKE_COMMAND} --build . --target wiiu_demo
    COMMAND WIILOAD=tcp:${WIILOAD_IP} /opt/devkitpro/tools/bin/wiiload "${CMAKE_CURRENT_BINARY_DIR}/wiiu_demo.rpx"
)

# "Deploy" target for copying assets to the Wii U using FTP, then running wiiload
# Deploy path is fixed (/fs/vol/external01/), IP is same as WIILOAD_IP
add_custom_target(deploy
    COMMAND ${CMAKE_COMMAND} --build . --target install
    COMMAND find "${CMAKE_INSTALL_PREFIX}/assets" -type f -exec curl -z '{}' -C - -T '{}' "ftp://${WIILOAD_IP}/fs/vol/external01/assets/" --ftp-create-dirs '\\;'
    COMMAND find "${CMAKE_INSTALL_PREFIX}/shaders" -type f -exec curl -z '{}' -C - -T '{}' "ftp://${WIILOAD_IP}/fs/vol/external01/shaders/" --ftp-create-dirs '\\;'
    COMMAND find "${CMAKE_INSTALL_PREFIX}/sync_tracks" -type f -exec curl -z '{}' -C - -T '{}' "ftp://${WIILOAD_IP}/fs/vol/external01/sync_tracks/" --ftp-create-dirs '\\;'
    COMMAND find "${CMAKE_INSTALL_PREFIX}/shader_cache" -type f -exec curl -z '{}' -C - -T '{}' "ftp://${WIILOAD_IP}/fs/vol/external01/shader_cache/" --ftp-create-dirs '\\;'
    COMMAND ${CMAKE_COMMAND} --build . --target wiiload
)

# hacky fix for emulator breaking when asset loading for some reason
set(EMULATOR_FIXES "true" CACHE BOOL "Set to false to disable kludges needed to run on emulator")
if(EMULATOR_FIXES)
    target_compile_definitions(wiiu_demo PRIVATE UFBX_TRACE=1)
    target_compile_definitions(wiiu_demo PRIVATE EMU_KLUDGES=1)
endif()

# target that clears the install directory for a fresh install
add_custom_target(clean_install
    COMMAND rm -rf "${CMAKE_INSTALL_PREFIX}/*"
)

# passthrough for SHADER_BUILD
set(SHADER_BUILD "false" CACHE BOOL "Set to true to build shaders and quit")
if(SHADER_BUILD)
    target_compile_definitions(wiiu_demo PRIVATE SHADER_BUILD=1)
endif()

# passthrough for WUHB_BUILD
set(WUHB_BUILD "false" CACHE BOOL "Set to true to build for WUHB package")
if(WUHB_BUILD)
    target_compile_definitions(wiiu_demo PRIVATE WUHB_BUILD=1)
endif()

# target that builds the wuhb package
# sorry about using cmake as a shell script
add_custom_target(wuhb
    # 1) make clean_install to clear out install directory
    COMMAND ${CMAKE_COMMAND} --build . --target clean_install

    # 2) make test, to build shader cache, define -DSHADER_BUILD=1 so it does that and then quits
    COMMAND ${CMAKE_COMMAND} -DSHADER_BUILD=ON -DWUHB_BUILD=OFF . 
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND ${CMAKE_COMMAND} --build . --target test
    COMMAND ${CMAKE_COMMAND} -DSHADER_BUILD=OFF -DWUHB_BUILD=OFF .

    # 3) make install, with WUHB_BUILD=1 defined, which changes paths to point to romfs and disables the shader compiler
    COMMAND ${CMAKE_COMMAND} -DSHADER_BUILD=OFF -DWUHB_BUILD=ON .
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND ${CMAKE_COMMAND} --build . --target install 
    COMMAND ${CMAKE_COMMAND} -DSHADER_BUILD=OFF -DWUHB_BUILD=OFF .

    # 4) copy assets, sync_tracks, shaders and shader_cache from install directory to a new directory called "content"
    COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/content"
    COMMAND cp -r "${CMAKE_INSTALL_PREFIX}/assets" "${CMAKE_CURRENT_BINARY_DIR}/content"
    COMMAND cp -r "${CMAKE_INSTALL_PREFIX}/shaders" "${CMAKE_CURRENT_BINARY_DIR}/content"
    COMMAND cp -r "${CMAKE_INSTALL_PREFIX}/sync_tracks" "${CMAKE_CURRENT_BINARY_DIR}/content"
    COMMAND cp -r "${CMAKE_INSTALL_PREFIX}/shader_cache" "${CMAKE_CURRENT_BINARY_DIR}/content"

    # 5) now we can build the wuhb package
    COMMAND wuhbtool --content content --short-name '${WUHB_TITLE}' --name '${WUHB_LONGTITLE}' --author '${WUHB_AUTHOR}' --icon '${WUHB_ICON}' "${CMAKE_CURRENT_BINARY_DIR}/wiiu_demo.rpx" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.wuhb"

    # 6) clean up the content directory
    COMMAND rm -rf "${CMAKE_CURRENT_BINARY_DIR}/content"
)

add_custom_target(wuhb_deploy
    COMMAND ${CMAKE_COMMAND} --build . --target wuhb
    COMMAND curl -T "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.wuhb" "ftp://${WIILOAD_IP}/fs/vol/external01/wiiu/apps/"
)
