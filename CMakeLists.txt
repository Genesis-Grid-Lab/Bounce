cmake_minimum_required(VERSION 3.5)
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
set(WINDOW_BACKEND "glfw" CACHE STRING "Window backend: glfw or native")
set(USE_FETCH_GLFW_DEFAULT OFF)
if (DEFINED USE_FECTH_GLFW)
  set(USE_FETCH_GLFW_DEFAULT ${USE_FECTH_GLFW})
endif()
option(USE_FETCH_GLFW "Fetch GLFW via FetchContent" ${USE_FETCH_GLFW_DEFAULT})

if (USE_FETCH_GLFW)
  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
  )
  FetchContent_MakeAvailable(glfw)
endif()

add_subdirectory(vendor/glm)
add_subdirectory(engine)
add_subdirectory(app)
