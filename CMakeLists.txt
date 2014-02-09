cmake_minimum_required(VERSION 2.8.8)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)


add_definitions("--std=c++11")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

include_directories(include)


##################################################
# shader library

add_library(
  shader SHARED
  src/shader.cxx
)

set_target_properties(
  shader PROPERTIES
  SOVERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
)

##################################################
# graphics-common library
add_library(
  graphics-common SHARED
  src/glew-common.cxx src/gl-common.cxx src/glfw-common.cxx
)

target_link_libraries(graphics-common shader)

set_target_properties(
  graphics-common PROPERTIES
  SOVERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
)
