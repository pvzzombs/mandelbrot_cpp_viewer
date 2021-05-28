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
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    MESSAGE("-- GNU Toolchain detected")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(MSVC_PATCH WIN32)
    MESSAGE("-- MSVC Toolchain detected")
    set ( CXX_STANDARD 11 )
  endif()
else()
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    MESSAGE("-- GNU Toolchain detected")
  else()
    set(CXX_STANDARD 11)
  endif()
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

# Only for LINK_MANDELBROT_STATIC
if(LINK_MANDELBROT_STATIC)
  include_directories("SFML/include")
  link_directories("${SFML_DIR}/lib")
endif(LINK_MANDELBROT_STATIC)

# Compile executable
add_executable(Mandelbrot ${MSVC_PATCH}  main.cpp TinyBitmapOut.cpp)
add_executable(Test ${MSVC_PATCH} test/test.cpp)

# Set include directory search paths
target_include_directories(Mandelbrot 
  PRIVATE
  "${PROJECT_BINARY_DIR}"
  )
target_include_directories(Test 
  PRIVATE
  "${PROJECT_BINARY_DIR}"
  )

if(WIN32)
  # Link executable to required SFML libraries
  target_link_libraries(Mandelbrot sfml-graphics)
  target_link_libraries(Test sfml-graphics)
else()
  if(LINK_MANDELBROT_STATIC)
    MESSAGE("-- Using static SFML libraries")
    target_link_libraries(Mandelbrot freetype GL udev sfml-graphics-s sfml-window-s sfml-system-s X11 pthread)
    target_link_libraries(Test freetype GL udev sfml-graphics-s sfml-window-s sfml-system-s X11 pthread)
  else()
    MESSAGE("-- Using dynamic SFML libraries")
    target_link_libraries(Mandelbrot X11 pthread sfml-graphics sfml-window sfml-system)
    target_link_libraries(Test X11 pthread sfml-graphics sfml-window sfml-system)
  endif(LINK_MANDELBROT_STATIC)
endif(WIN32)

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
else()
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/verdana.ttf" "${CMAKE_CURRENT_BINARY_DIR}\\verdana.ttf" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\config.txt" COPYONLY)
  endif()
endif(WIN32)
# Install target
#install(TARGETS Mandelbrot DESTINATION bin)

