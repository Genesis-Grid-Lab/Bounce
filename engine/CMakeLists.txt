project(Engine LANGUAGES CXX)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode.")
    # Add debug-specific flags or definitions here
    add_compile_definitions(E_DEBUG)
else()
    message(STATUS "Building in Release or other mode.")
endif()

if(WINDOW_BACKEND_GLFW)
  file(GLOB_RECURSE nativeFile)
  file(GLOB_RECURSE androidFile)
  file(GLOB_RECURSE iosFile)
  file(GLOB_RECURSE glfwFile ${CMAKE_CURRENT_SOURCE_DIR}/src/Window/GLFW/*.cpp)
elseif(WINDOW_BACKEND STREQUAL "native")
  file(GLOB_RECURSE nativeFile ${CMAKE_CURRENT_SOURCE_DIR}/src/window/native/*.cpp)
  file(GLOB_RECURSE glfwFile)
  file(GLOB_RECURSE androidFile)
  file(GLOB_RECURSE iosFile)
elseif(WINDOW_BACKEND STREQUAL "android")
  file(GLOB_RECURSE nativeFile)
  file(GLOB_RECURSE glfwFile)
  file(GLOB_RECURSE androidFile  ${CMAKE_CURRENT_SOURCE_DIR}/src/window/android/*.cpp)
  file(GLOB_RECURSE iosFile)
elseif(WINDOW_BACKEND STREQUAL "ios")
  file(GLOB_RECURSE nativeFile)
  file(GLOB_RECURSE glfwFile)
  file(GLOB_RECURSE androidFile)
  file(GLOB_RECURSE iosFile  ${CMAKE_CURRENT_SOURCE_DIR}/src/window/ios/*.cpp)
else()
  message(FATAL_ERROR "Unknown WINDOW_BACKEND: ${WINDOW_BACKEND}")
endif()


file(GLOB core ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.cpp)
file(GLOB render ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/*.cpp)
file(GLOB scene ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene/*.cpp)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE vulkanFiles ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Vulkan/*.cpp)
file(GLOB gladFile ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/glad/*.cpp)
add_library(${PROJECT_NAME}
  ${core} ${headers}
  ${render}
  ${glfwFile}
  ${nativeFile}
  ${iosFile}
  ${androidFile}
  ${vulkanFiles}
  ${gladFile}
  ${scene}
  )

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/epch.h)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/vendor/spdlog/include
  ${CMAKE_SOURCE_DIR}/vendor/glm
  ${CMAKE_SOURCE_DIR}/vendor/glad/include
  ${CMAKE_SOURCE_DIR}/vendor/stb_image
  ${CMAKE_SOURCE_DIR}/vendor/imgui
  ${CMAKE_SOURCE_DIR}/vendor/imgui/backends
  ${CMAKE_SOURCE_DIR}/vendor/assimp/include
  ${CMAKE_SOURCE_DIR}/vendor/entt/src/entt
  ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  glfw
  glm
  EnTT
  glad
  assimp
  imgui
  spdlog
)

if (Vulkan_FOUND)
  target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
else()
  # Using volk (no prototypes from headers)
  target_compile_definitions(${PROJECT_NAME} PRIVATE VK_NO_PROTOTYPES)
  target_link_libraries(${PROJECT_NAME} PRIVATE volk)
  target_include_directories(${PROJECT_NAME} PRIVATE ${vulkan_headers_SOURCE_DIR}/include)
endif()

# copy data
if(EXISTS ${CMAKE_SOURCE_DIR}/Data)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND 
        ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_SOURCE_DIR}/Data
        ${EXECUTABLE_OUTPUT_PATH}/Data
    )
else()
    message(WARNING "[WARNING] no data directory!")
  endif()

# copy resources
if(EXISTS ${CMAKE_SOURCE_DIR}/Resources)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND 
        ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_SOURCE_DIR}/Resources
        # ${EXECUTABLE_OUTPUT_PATH}/Resources
	$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
    )
else()
    message(WARNING "[WARNING] no resources directory!")
endif()
