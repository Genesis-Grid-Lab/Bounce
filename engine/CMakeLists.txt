project(Engine LANGUAGES CXX)

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


file(GLOB source ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.cpp)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(${PROJECT_NAME}
    ${source} ${headers}
    ${glfwFile}
    ${nativeFile}
    ${iosFile}
    ${androidFile}
  )

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/epch.h)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  glfw
)

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
