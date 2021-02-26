cmake_minimum_required(VERSION 3.1)

# Set project name and version
project(Mandelbrot)


MESSAGE("-- Release Mode")
set(CMAKE_BUILD_TYPE Release 
  CACHE STRING 
    "Choose the type of build (Debug or Release)" 
    FORCE
)

# Set CXX_STANDARD
if(WIN32)
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MINGW")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    MESSAGE("-- GNU Toolchain detected")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(MSVC_PATCH WIN32)
    MESSAGE("-- MSVC Toolchain detected")
    set ( CXX_STANDARD 14 )
  endif()
else()
  set(CXX_STANDARD 14)
endif(WIN32)

# Generate config.h
#configure_file(config.h.in config.h)

# Set SFML_DIR search path, you can set this option 
# configuring cmake
if(WIN32)
  set(SFML_DIR "SFML\\build\\" CACHE FILE_PATH "SFML\\build\\" FORCE)
else()
  set(SFML_DIR "SFML/build/" CACHE FILE_PATH "SFML/build/" FORCE)
endif(WIN32)


# Find SFML shared libraries
find_package(SFML 2.5 
  COMPONENTS 
    system window graphics network audio REQUIRED
  )

# Compile executable
add_executable(Mandelbrot ${MSVC_PATCH}  main.cpp TinyBitmapOut.cpp)

# Set include directory search paths
target_include_directories(Mandelbrot 
  PRIVATE
  "${PROJECT_BINARY_DIR}"
  )

# Link executable to required SFML libraries
target_link_libraries(Mandelbrot sfml-graphics)

if(WIN32)
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    if("${CMAKE_GENERATOR}" STREQUAL "NMake Makefiles")
      configure_file("${SFML_DIR}\\lib\\sfml-system-2.dll" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-system-2.dll" COPYONLY)
      configure_file("${SFML_DIR}\\lib\\sfml-window-2.dll" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-window-2.dll" COPYONLY)
      configure_file("${SFML_DIR}\\lib\\sfml-graphics-2.dll" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-graphics-2.dll" COPYONLY)
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\verdana.ttf" "${CMAKE_CURRENT_BINARY_DIR}\\verdana.ttf" COPYONLY)
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\config.txt" COPYONLY)
    else()
      configure_file("${SFML_DIR}\\lib\\Release\\sfml-system-2.dll" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\sfml-system-2.dll" COPYONLY)
      configure_file("${SFML_DIR}\\lib\\Release\\sfml-window-2.dll" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\sfml-window-2.dll" COPYONLY)
      configure_file("${SFML_DIR}\\lib\\Release\\sfml-graphics-2.dll" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\sfml-graphics-2.dll" COPYONLY)
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\verdana.ttf" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\verdana.ttf" COPYONLY)
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\config.txt" COPYONLY)
    endif()
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MINGW")
    configure_file("${SFML_DIR}\\lib\\sfml-system-2.dll" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-system-2.dll" COPYONLY)
    configure_file("${SFML_DIR}\\lib\\sfml-window-2.dll" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-window-2.dll" COPYONLY)
    configure_file("${SFML_DIR}\\lib\\sfml-graphics-2.dll" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-graphics-2.dll" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\verdana.ttf" "${CMAKE_CURRENT_BINARY_DIR}\\verdana.ttf" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\config.txt" COPYONLY)
  endif()
endif(WIN32)
# Install target
#install(TARGETS Mandelbrot DESTINATION bin)

