cmake_minimum_required(VERSION 3.12)
project(SocketDemo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(OPENGL_VERSION_MAJOR 4)
set(OPENGL_VERSION_MINOR 5)

# GLFW ----------------------------------------------------------------------------------------
    # Dont build any examples, tests or documentation
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    add_subdirectory(dependencies/glfw dependencies/glfw/CMake)
# GLFW end ------------------------------------------------------------------------------------

# ImGui ----------------------------------------------------------------------------------------
add_library(ImGui
    dependencies/imgui/imgui_demo.cpp
    dependencies/imgui/imgui_draw.cpp
    dependencies/imgui/imgui_tables.cpp
    dependencies/imgui/imgui_widgets.cpp
    dependencies/imgui/imgui.cpp
    dependencies/imgui/backends/imgui_impl_opengl3.cpp
    dependencies/imgui/backends/imgui_impl_glfw.cpp
    dependencies/imgui/misc/cpp/imgui_stdlib.cpp
    )


# GLAD ----------------------------------------------------------------------------------------
    add_subdirectory(dependencies/glad/cmake dependencies/glad)
    glad_add_library(glad API gl:core=${OPENGL_VERSION_MAJOR}.${OPENGL_VERSION_MINOR})
# GLAD end ------------------------------------------------------------------------------------

if (MSVC)
    set(CMAKE_CXX_FLAGS "/EHsc")
endif(MSVC)

add_executable(client source/client.cpp source/GUI.cpp source/networking.cpp source/networking.h)

target_include_directories(ImGui
    PUBLIC dependencies/imgui
    PUBLIC dependencies/imgui/misc/cpp
    PUBLIC dependencies/glfw/include
)

if (WIN32)
	target_link_libraries(client Ws2_32.lib Mswsock.lib AdvApi32.lib)
endif()
target_link_libraries(client ImGui)
target_link_libraries(client glfw)
target_link_libraries(client glad)


