cmake_minimum_required(VERSION 2.8)
project(graffiks)
set(CMAKE_BUILD_TYPE Release)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_definitions(-DRESOURCE_PATH="${CMAKE_INSTALL_PREFIX}/share/graffiks/")

file(GLOB base_SRC
  "src/*.c"
  "src/mesh/*.c"
  "src/material/*.c"
  "src/object/*.c"
  "src/renderer/*.c"
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX)
  set(DRIVER src/driver/driver-linux.c)
  set(OS linux)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/${OS}/)

add_library(graffiks SHARED ${base_SRC} ${DRIVER})

find_library(GLU_LIB GLU)
find_library(GL_LIB GL)
find_library(GLEW_LIB GLEW)
find_library(C_LIB c)
find_library(M_LIB m)
find_library(X11_LIB X11)
target_link_libraries(graffiks ${C_LIB} ${M_LIB} ${X11_LIB} ${GL_LIB} ${GLU_LIB} ${GLEW_LIB})

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
