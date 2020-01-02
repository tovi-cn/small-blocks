cmake_minimum_required(VERSION 3.0)

project(recurse-builder)

add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/glfw)
set(GLM_TEST_ENABLE OFF CACHE BOOL "")
add_subdirectory(dependencies/glm)

include_directories(dependencies/glm)
include_directories(dependencies/glad/include)
include_directories(dependencies/glfw/include)

set(CMAKE_CXX_STANDARD 11)
if (MSVC)
  add_compile_options(/W3)
else()
  add_compile_options(-Wall -Wextra)
endif()

add_executable(recurse-builder
  src/block.cc
  src/game.cc
  src/geometry.cc
  src/main.cc
  )
target_link_libraries(recurse-builder glad)
target_link_libraries(recurse-builder glfw ${GLFW_LIBRARIES})
