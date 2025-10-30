cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# build output directories

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/lib)
set(CMAKE_PDB_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH $<0:>${CMAKE_BINARY_DIR}/bin)

project(Bounce LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Options
option(USE_SYSTEM_VULKAN "Use system-installed Vulkan loader if available" OFF)
option(USE_VULKAN "Use vulkan" OFF)
option(USE_GLAD "Use glad" ON)
option(PHYSICS_BACKEND_JOLT "Use Jolt physics" ON)
option(PHYSICS_BACKEND_BULLET3 "Use bullet physics" ON)
option(WINDOW_BACKEND_GLFW "Use GLFW" ON)
option(WINDOW_BACKEND_NATIVE "Use native window" OFF)
set(USE_FETCH_GLFW_DEFAULT OFF)
if (DEFINED USE_FECTH_GLFW)
  set(USE_FETCH_GLFW_DEFAULT ${USE_FECTH_GLFW})
endif()
option(USE_FETCH_GLFW "Fetch GLFW via FetchContent" ${USE_FETCH_GLFW_DEFAULT})

if (USE_SYSTEM_VULKAN)
  find_package(Vulkan QUIET)  # provides Vulkan::Vulkan and Vulkan::Headers (on newer CMake/SDKs)
  set(USE_VULKAN ON)
endif()

if (NOT Vulkan_FOUND)
  message(STATUS "Vulkan not found or active")
endif()

if (USE_FETCH_GLFW)
  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
  )
  FetchContent_MakeAvailable(glfw)
endif()

if(USE_GLAD)
  add_subdirectory(vendor/glad)
endif()

add_subdirectory(vendor/glm)
add_subdirectory(vendor/entt)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/assimp)
if(PHYSICS_BACKEND_JOLT)
  set(PHYSICS_BACKEND_BULLET3 OFF)
  add_subdirectory(vendor/JoltPhysics/Build)
endif()
if(PHYSICS_BACKEND_BULLET3)
  set(USE_DOUBLE_PRECISION ON)
  set(BUILD_OPENGL3_DEMOS OFF)
  set(BUILD_BULLET2_DEMOS OFF)
  set(BUILD_UNIT_TESTS OFF)
  add_subdirectory(vendor/bullet3)
endif()
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/imgui)
add_subdirectory(engine)
add_subdirectory(app)
