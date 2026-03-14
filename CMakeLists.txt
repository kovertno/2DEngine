cmake_minimum_required(VERSION 3.20)
project(gameengine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wfatal-errors")

include_directories(./libs/)

execute_process(
        COMMAND sdl2-config --cflags
        OUTPUT_VARIABLE SDL2_CFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND sdl2-config --libs
        OUTPUT_VARIABLE SDL2_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SDL2_CFLAGS}")

file(GLOB SOURCES
        "src/*.cpp"
        "src/Game/*.cpp"
        "src/Logger/*.cpp"
        "src/ECS/*.cpp"
        "src/AssetManager/*.cpp"
        "libs/imgui/*.cpp"
)

add_executable(gameengine ${SOURCES}
        src/Components/CameraFollowComponent.h
        src/Systems/CameraMovementSystem.h
        src/Components/ProjectileEmitterComponent.h
        src/Systems/ProjectileEmitSystem.h
        src/Components/HealthComponent.h
        src/Components/ProjectileComponent.h
        src/Systems/ProjectileLifeCycleSystem.h
        src/Components/TextLabelComponent.h
        src/Systems/RenderTextSystem.h
        src/Systems/RenderHealthBarSystem.h
        libs/imgui/imgui_impl_sdl2.h
        libs/imgui/imgui_impl_sdl2.cpp
        src/Systems/RenderGUISystem.h
        src/Game/LevelLoader.h
        src/Game/LevelLoader.cpp
        src/Components/ScriptComponent.h
        src/Systems/ScriptSystem.h)

target_link_libraries(gameengine
        ${SDL2_LIBS}
        SDL2_image
        SDL2_ttf
        SDL2_mixer
        lua
)

if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    add_custom_command(TARGET gameengine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:gameengine>/assets
    )
endif()