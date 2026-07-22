cmake_minimum_required(VERSION 3.16)
project(ECHO C)

set(CMAKE_C_STANDARD 99)

# Hard size optimization flags for demoscene target (<= 1.44 MB)
if(MSVC)
    add_compile_options(/O1 /Os /GL /Gy /Gw)
    add_link_options(/LTCG /OPT:REF /OPT:ICF)
else()
    add_compile_options(-Os -s -ffunction-sections -fdata-sections -Wall)
    add_link_options(-Wl,--gc-sections -s)
endif()

# Raylib minimal build configuration flags
set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_PNG OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_JPG OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_WAV OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_MP3 OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_OGG OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_FLAC OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_XM OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_MOD OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_TTF OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_OBJ OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_MTL OFF CACHE BOOL "" FORCE)

# Find system Raylib or FetchContent minimal Raylib
find_package(raylib 4.0 QUIET)

if(NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Application Source Files
set(SOURCES
    main.c
    game.c
    world.c
    particles.c
    audio.c
)

add_executable(echo ${SOURCES})

target_include_directories(echo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(echo PRIVATE raylib)

if(WIN32)
    target_link_libraries(echo PRIVATE opengl32 gdi32 winmm user32 shell32)
endif()

# Output size report target
add_custom_command(TARGET echo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "--- ECHO Executable Build Complete ---"
)
