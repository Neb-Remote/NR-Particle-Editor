cmake_minimum_required(VERSION 3.25)

project(ImGuiFileDialog)

add_library(ImGuiFileDialog STATIC
        ImGuiFileDialog.cpp
        ImGuiFileDialog.h
        ImGuiFileDialogConfig.h
)

set(IMGUI_PUBLIC_HEADERS
        ${IMGUI_INCLUDE_DIR}/imconfig.h
        ${IMGUI_INCLUDE_DIR}/imgui.h
        ${IMGUI_INCLUDE_DIR}/imgui_internal.h # not actually public, but users might need it
        ${IMGUI_INCLUDE_DIR}/imstb_rectpack.h
        ${IMGUI_INCLUDE_DIR}/imstb_textedit.h
        ${IMGUI_INCLUDE_DIR}/imstb_truetype.h
        ${IMGUI_INCLUDE_DIR}/misc/cpp/imgui_stdlib.h
)

target_include_directories(ImGuiFileDialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} $<BUILD_INTERFACE:${IMGUI_INCLUDE_DIR}>)

if (UNIX)
    target_compile_options(ImGuiFileDialog PUBLIC "-Wno-unknown-pragmas")
endif ()
