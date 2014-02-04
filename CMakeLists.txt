cmake_minimum_required(VERSION 2.8.8)

find_package(OpenGL REQUIRED)

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
  NOVERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
)

# ##################################################
# # glew-common library
# 
# add_library(
#   glew-common SHARED
#   src/glew-common.cxx
# )
# 
# set_target_properties(
#   shader PROPERTIES
#   NOVERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
# )
# 
# ##################################################
# # gl-common library
# add_library(
#   gl-common SHARED
#   src/gl-common.cxx
# )
# 
# set_target_properties(
#   gl-common PROPERTIES
#   NOVERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
# )
# 
# ##################################################
# # glfw-common library
# add_library(
#   glfw-common SHARED
#   src/glfw-common.cxx
# )
# 
# set_target_properties(
#   glfw-common PROPERTIES
#   NOVERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
# )

##################################################
# graphics-common library
add_library(
  graphics-common SHARED
  src/glew-common.cxx src/gl-common.cxx src/glfw-common.cxx
)

target_link_libraries(graphics-common shader)

set_target_properties(
  graphics-common PROPERTIES
  NOVERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
)
