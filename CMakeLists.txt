cmake_minimum_required(VERSION 3.16)
project(upgrader LANGUAGES CXX)

# ---- Global settings ----
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Dependencies ----
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)


include(FetchContent)

FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG main
)

FetchContent_MakeAvailable(entt)

# ---- ImGui ----
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    # Backends
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(imgui
    PUBLIC glfw OpenGL::GL
)

# ---- Main executable ----
add_executable(upgrader
    src/main.cpp
    src/Game.cpp
    src/Text.cpp
    src/Currency.cpp
    src/Upgrades.cpp
    src/UpgradeFactory.cpp
    src/Resources.cpp
    src/User.cpp
)

target_compile_definitions(upgrader
    PRIVATE $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(upgrader SYSTEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
)

target_include_directories(upgrader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(upgrader
    PRIVATE
    imgui
    EnTT::EnTT
)

target_compile_options(upgrader PRIVATE -Wall -Wextra)

# ---- Tools ----
add_executable(bitshifter src/bitshifter.cpp)
target_include_directories(bitshifter PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(bitshifter PRIVATE EnTT::EnTT)

add_executable(packer src/packer.cpp)
