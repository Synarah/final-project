file(GLOB SRC "*.h" "*.cpp" "mesh/*.h" "mesh/*.cpp") 
add_library(agl STATIC ${SRC})

set(CMAKE_MACOSX_RPATH 1)
  set(CMAKE_CXX_FLAGS "-Wall -Wno-deprecated-declarations -Wno-reorder-ctor -Wno-unused-variable -g -stdlib=libc++ -std=c++14")
  set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
  find_library(GL_LIB OpenGL)
  find_library(GLFW glfw)
  add_definitions(-DAPPLE)
  add_definitions('--std=c++11')

  set(INCLUDE_DIRS
    /System/Library/Frameworks
     ./src/agl
     ./src
    ./external/include
    /usr/local/include)

  set(CORE ${GLFW} ${GL_LIB})


include_directories(${INCLUDE_DIRS})
link_directories(${LIBRARY_DIRS})

file(GLOB AGLSRC "src/agl/*.h" "src/agl/*.cpp" "src/agl/mesh/*.h" "src/agl/mesh/*.cpp")

set(SOURCES
    ${AGLSRC}
    src/plymesh.cpp
    src/plymesh.h
    src/osutils.h
    src/osutils.cpp )

set(SHADERS
    shaders/billboard.fs
    shaders/billboard.vs
    shaders/cubemap.fs
    shaders/cubemap.vs
    shaders/text.fs
    shaders/text.vs
    shaders/unlit.fs
    shaders/unlit.vs)

add_executable(test-ply-mesh src/test-ply-mesh.cpp ${SOURCES} ${SHADERS})
target_link_libraries(test-ply-mesh ${CORE})

add_executable(mesh-viewer src/mesh-viewer.cpp ${SOURCES} ${SHADERS})
target_link_libraries(mesh-viewer ${CORE})
