project(Game LANGUAGES CXX)

file(GLOB source ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_executable(${PROJECT_NAME}
    ${source} ${headers}
  )

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/engine/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  Engine
)
