cmake_minimum_required(VERSION 2.8)
project(graffiks)
set(CMAKE_BUILD_TYPE Release)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_definitions(-DRESOURCE_PATH="${CMAKE_INSTALL_PREFIX}/share/graffiks/")
#set(CMAKE_C_FLAGS "-g -fsanitize=address")

file(GLOB base_SRC
  "src/*.c"
  "src/mesh/*.c"
  "src/object/*.c"
  "src/renderer/*.c"
)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
if(NOT GLEW_FOUND)
  message(ERROR, " GLEW not found!")
endif(NOT GLEW_FOUND)

find_library(C_LIB c)
find_library(M_LIB m)
find_library(X11_LIB X11)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX)
  set(DRIVER src/driver/driver-linux.c)
  set(OS linux)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(C_LIB "")
  set(M_LIB "")
  set(X11_LIB "")
  set(DRIVER src/driver/driver-windows.c)
  add_definitions(-DGRAFFIKS_BUILD_SHARED)
  set(OS windows)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/${OS}/)

add_library(graffiks SHARED ${base_SRC} ${DRIVER})

target_link_libraries(graffiks ${C_LIB} ${M_LIB} ${X11_LIB} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/share/
    DESTINATION "share/graffiks"
    DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/${OS}/
    DESTINATION "lib"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION "include/"
    DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
