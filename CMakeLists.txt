cmake_minimum_required(VERSION 3.11)

project(COMP371_Project)

set(CMAKE_CXX_STANDARD 11)

include(ExternalProject)

# Set install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH ${CMAKE_SOURCE_DIR}/dist FORCE)
if (WIN32)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Debug;Release" FORCE)
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

include(BuildGLEW)
include(BuildGLFW)
include(BuildGLM)
include(BuildFreeType)

# main
set(EXEC main)

file(GLOB SRC src/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${EXEC} ${SRC})

target_include_directories(${EXEC} PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/include)
target_link_libraries(${EXEC} OpenGL::GL glew_s glfw glm freetype)

if (MSVC)

endif (MSVC)

if (UNIX)
    #    add_compile_options(-L/Users/hugozheng/CLionProjects/COMP371_Project/thirdparty/irrKlang-64bit-1.6.0/macosx-gcc)
endif (UNIX)
#message("${COMPILE_OPTIONS}")
add_library(irrklang SHARED IMPORTED)
set_property(TARGET irrklang PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/thirdparty/irrKlang-64bit-1.6.0/macosx-gcc/libirrklang.dylib)
target_link_libraries(${EXEC} irrklang)

list(APPEND BIN ${EXEC})

# install files to install location
install(TARGETS ${BIN} DESTINATION ${CMAKE_INSTALL_PREFIX})

