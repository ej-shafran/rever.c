# --- Project settings ---

cmake_minimum_required(VERSION 3.13)
project(reverc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(REVERC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(REVERC_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(REVERC_VERSION 1.0)

# --- Raylib ---

set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # check if raylib already exists
if (NOT raylib_FOUND) # if there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

# --- Reverc ---

if (NOT TARGET uninstall)
  add_custom_target(
    uninstall
    COMMENT "Uninstall installed files"
    COMMAND xargs rm -vf < install_manifest.txt
  )
endif()

# CLI

add_executable(${PROJECT_NAME}-cli)
install(TARGETS ${PROJECT_NAME}-cli)

set_target_properties(${PROJECT_NAME}-cli PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-cli)

# GUI

add_executable(${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} raylib m)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

add_subdirectory(src)
