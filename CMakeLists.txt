cmake_minimum_required(VERSION 3.0)

project(small-blocks)

add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/stb)

include_directories(dependencies)
include_directories(dependencies/glm)
include_directories(dependencies/glad/include)
include_directories(dependencies/glfw/include)
include_directories(dependencies/stb)
include_directories(src)

set(CMAKE_CXX_STANDARD 11)
if (MSVC)
  add_compile_options(/W3)
else()
  add_compile_options(-Wall -Wextra)
endif()

add_executable(small-blocks
  src/block.cc
  src/fractals.cc
  src/game.cc
  src/geometry.cc
  src/input.cc
  src/main.cc
  src/material.cc
  src/mesh.cc
  src/physics.cc
  src/renderer.cc
  src/utilities.cc
  src/window.cc
  )

target_link_libraries(small-blocks
  glad
  glfw
  ${GLFW_LIBRARIES}
  stb_image
  )
