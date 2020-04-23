cmake_minimum_required (VERSION 3.15)
project (opengl_particle_system)

set (CMAKE_CXX_STANDARD 11)

#	Particle system includes
include_directories (include)

#	External includes
include_directories (external/include/GL)
include_directories (external/include/GLFW)
include_directories (external/include/glm)
include_directories (external/include/PNG)

#	Collect all source files
file (GLOB source "source/*.cpp")

#	Compile source code
add_executable (particle ${source})

#	Find lib files
find_library (GLEW glew32 external/libs)
find_library (OPENGL opengl32 external/libs)
find_library (GDI gdi32 external/libs)
find_library (GLFW glfw3dll external/libs)
find_library (PNG png15.dll external/libs)

#	Link OBJ files
target_link_libraries (particle LINK_PUBLIC ${GLEW} ${OPENGL} ${GDI} ${GLFW} ${PNG})

#	Install the software
file (GLOB DYNAMIC "dlls/*.dll")
file (GLOB SHADERS "resources/shaders/*")
file (GLOB TEXTURES "resources/textures/*.png")

install (TARGETS particle DESTINATION bin)
install (FILES ${DYNAMIC} DESTINATION bin)
install (FILES ${SHADERS} DESTINATION bin/shaders)
install (FILES ${TEXTURES} DESTINATION bin/textures)
