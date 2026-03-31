cmake_minimum_required(VERSION 3.20)
project(Strawberry2D)
enable_testing()
include(FetchContent)
include(ExternalProject)


if (PROJECT_IS_TOP_LEVEL)
    FetchContent_Declare(StrawberryConfig
            GIT_REPOSITORY "https://github.com/StrawberryDan/Config.git"
            GIT_TAG "main")
    FetchContent_MakeAvailable(StrawberryConfig)
    include(StrawberryConfig)
endif ()


if (NOT TARGET Strawberry2D)
    find_strawberry_library(NAMES Core Window Vulkan)
    new_strawberry_library(NAME Strawberry2D SOURCE
        src/Strawberry/2D/NormalSprite.hpp
        src/Strawberry/2D/Sprite.cpp
        src/Strawberry/2D/Sprite.hpp
        src/Strawberry/2D/SpriteRenderer.cpp
        src/Strawberry/2D/SpriteRenderer.hpp
        src/Strawberry/2D/SpriteTransform.hpp
        src/Strawberry/2D/TextureAtlas.cpp
        src/Strawberry/2D/TextureAtlas.hpp
        src/Strawberry/2D/TextureReference.cpp
        src/Strawberry/2D/TextureReference.hpp
        src/Strawberry/2D/TextureManifest.hpp
    )
    target_include_directories(Strawberry2D PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(Strawberry2D PUBLIC StrawberryCore StrawberryWindow StrawberryVulkan)
endif ()