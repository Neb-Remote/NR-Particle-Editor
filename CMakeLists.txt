cmake_minimum_required(VERSION 3.25)
project(nr-particle-editor CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS false)

add_subdirectory(external)

option(SYMLINK_PATHS "Sym link paths" ON)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.hpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(nr-particle-editor ${SRC_FILES})
    target_compile_definitions(nr-particle-editor PRIVATE NR_PARTICLE_EDITOR_DEBUG)
else ()
    # If we're on Windows and we want to use winmain signature without it polluting
    # the code, we need to link SFML main (this only applies to windows)
    if (WIN32)
        add_executable(nr-particle-editor WIN32 ${SRC_FILES})
        target_link_libraries(nr-particle-editor PRIVATE SFML::Main)
    else ()
        add_executable(nr-particle-editor ${SRC_FILES})
    endif ()
endif ()

if (WIN32)
    target_compile_definitions(nr-particle-editor PUBLIC NR_PARTICLE_EDITOR_WINDOWS)
endif ()

# When wanting to set an executable icon
# target_sources(nr-particle-editor PRIVATE src/appicon.rc)

target_compile_features(nr-particle-editor PUBLIC cxx_std_20)

target_link_libraries(nr-particle-editor PRIVATE SFML::Graphics ImGui-SFML::ImGui-SFML spdlog nlohmann_json expected ImGuiFileDialog)

if (MSVC)
    add_compile_options("/MP")
    target_compile_definitions(nr-particle-editor PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
    target_compile_options(nr-particle-editor PRIVATE /W4 /WX /permissive- /wd4068)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
    target_compile_options(nr-particle-editor PRIVATE -Werror -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wno-c++98-compat)
endif ()
