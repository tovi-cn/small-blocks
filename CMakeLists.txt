cmake_minimum_required(VERSION 3.0)

project(small-blocks)

# TODO: Build with freetype-gl
# set(freetype-gl_BUILD_DEMOS OFF CACHE BOOL "")
# set(freetype-gl_BUILD_APIDOC OFF CACHE BOOL "")
# set(freetype-gl_BUILD_MAKEFONT OFF CACHE BOOL "")
# set(freetype-gl_BUILD_TESTS OFF CACHE BOOL "")
# add_subdirectory(dependencies/freetype-gl)

add_subdirectory(dependencies/glad)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
add_subdirectory(dependencies/glfw)

set(GLM_TEST_ENABLE OFF CACHE BOOL "")
add_subdirectory(dependencies/glm)

include_directories(dependencies)
include_directories(dependencies/glm)
include_directories(dependencies/glad/include)
include_directories(dependencies/glfw/include)

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
  src/main.cc
  src/utilities.cc
  )
target_link_libraries(small-blocks
  freetype-gl
  glad
  glfw
  ${GLFW_LIBRARIES}
  )
