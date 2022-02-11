cmake_minimum_required(VERSION 3.12)

# Set project name and version
project(Mandelbrot)

#set(CMAKE_VERBOSE_MAKEFILE ON)

MESSAGE("-- Building is forced to be in Release mode...")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)

# Set CXX_STANDARD
if(WIN32)
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MINGW")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    MESSAGE("-- MinGW Toolchain detected")
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
# if(WIN32)
#   set(SFML_DIR "SFML\\build\\" CACHE FILE_PATH "SFML\\build\\" FORCE)
# else()
#   set(SFML_DIR "SFML/build/" CACHE FILE_PATH "SFML/build/" FORCE)
# endif(WIN32)


# Find SFML shared libraries
#find_package(SFML 2.5.1 
#  COMPONENTS 
#    system window graphics network audio REQUIRED
#  )

# Only for LINK_MANDELBROT_STATIC
#if(LINK_MANDELBROT_STATIC)
#  include_directories("include")
#  link_directories("lib")
#  MESSAGE("-- Static build detected")
#endif(LINK_MANDELBROT_STATIC)

# Executables to be compiled
add_executable(Mandelbrot ${MSVC_PATCH}  main.cpp TinyBitmapOut.cpp)
add_executable(Test ${MSVC_PATCH} test/test.cpp)

# Set include directory search paths
target_include_directories(Mandelbrot
  PRIVATE "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" "include" "SFML\\include" "SFML/include"
)
target_include_directories(Test
  PRIVATE "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" "include" "SFML\\include" "SFML/include"
)

if(LINK_MANDELBROT_STATIC)
  MESSAGE("-- Finding static libraries")
  find_library(FREETYPE_LIB
  NAMES "libfreetype.a" 
  HINTS "lib" "SFML\\build\\lib" "SFML/build/lib" "SFML\\extlibs\\libs-mingw" "SFML\\extlibs\\libs-msvc" "SFML\\extlibs\\libs-msvc"
  #PATHS "D:\\Documents\\mandelbrot_cpp_viewer\\lib"
  )

  find_library(SFML_GRAPHICS_LIB
  NAMES "libsfml-graphics-s.a"
  HINTS "lib" "SFML\\build\\lib" "SFML/build/lib"
  #PATHS "D:\\Documents\\mandelbrot_cpp_viewer\\lib"
  )

  find_library(SFML_WINDOW_LIB
  NAMES "libsfml-window-s.a"
  HINTS "lib" "SFML\\build\\lib" "SFML/build/lib"
  #PATHS "D:\\Documents\\mandelbrot_cpp_viewer\\lib"
  )

  find_library(SFML_SYSTEM_LIB
  NAMES "libsfml-system-s.a"
  HINTS "lib" "SFML\\build\\lib" "SFML/build/lib"
  #PATHS "D:\\Documents\\mandelbrot_cpp_viewer\\lib"
  )


else()
  find_library(SFML_GRAPHICS_LIB
  NAMES sfml-graphics
  HINTS "lib" "SFML\\build\\lib" "SFML/build/lib"
  #PATHS "D:\\Documents\\mandelbrot_cpp_viewer\\lib"
  )

  find_library(SFML_WINDOW_LIB
  NAMES sfml-window
  HINTS "lib" "SFML\\build\\lib" "SFML/build/lib"
  #PATHS "D:\\Documents\\mandelbrot_cpp_viewer\\lib"
  )

  find_library(SFML_SYSTEM_LIB
  NAMES sfml-system
  HINTS "lib" "SFML\\build\\lib" "SFML/build/lib"
  #PATHS "D:\\Documents\\mandelbrot_cpp_viewer\\lib"
  )

  # shared libraries
  find_file(SFML_GRAPHICS_DLL
  NAMES "sfml-graphics-2.dll" "libsfml-graphics.so"
  HINTS "lib" "SFML\\build\\lib" "SFML/build/lib"
  #PATHS "D:\\Documents\\mandelbrot_cpp_viewer\\lib"
  )

  find_file(SFML_WINDOW_DLL
  NAMES "sfml-window-2.dll" "libsfml-window.so"
  HINTS "lib" "SFML\\build\\lib" "SFML/build/lib"
  #PATHS "D:\\Documents\\mandelbrot_cpp_viewer\\lib"
  )

  find_file(SFML_SYSTEM_DLL
  NAMES "sfml-system-2.dll" "libsfml-system.so"
  HINTS "lib" "SFML\\build\\lib" "SFML/build/lib"
  #PATHS "D:\\Documents\\mandelbrot_cpp_viewer\\lib"
  )
  message("-- The shared libraries are ${SFML_GRAPHICS_DLL} ${SFML_WINDOW_DLL} ${SFML_SYSTEM_DLL}")
endif()

# Linking
if(LINK_MANDELBROT_STATIC)
  add_compile_definitions(SFML_STATIC)
  if(WIN32)
    MESSAGE("-- The static libraries to be linked at are ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} ${FREETYPE_LIB} opengl32 winmm gdi32")
    target_link_libraries(Mandelbrot ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} ${FREETYPE_LIB} opengl32 winmm gdi32)
    target_link_libraries(Test ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} ${FREETYPE_LIB} opengl32 winmm gdi32)
  else()
    MESSAGE("-- The static libraries to be linked at are freetype GL udev sfml-graphics-s sfml-window-s sfml-system-s X11 pthread")
    target_link_libraries(Mandelbrot freetype GL udev sfml-graphics-s sfml-window-s sfml-system-s X11 pthread)
    target_link_libraries(Test freetype GL udev sfml-graphics-s sfml-window-s sfml-system-s X11 pthread)
  endif()
else()
  MESSAGE("-- Using dynamic SFML libraries")
  if(WIN32)
    target_link_libraries(Mandelbrot ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB})
    target_link_libraries(Test ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB})
  else()
    target_link_libraries(Mandelbrot X11 pthread sfml-graphics sfml-window sfml-system)
    target_link_libraries(Test X11 pthread sfml-graphics sfml-window sfml-system)
  endif()
endif()

# Configuration files
if(LINK_MANDELBROT_STATIC)
  if(WIN32)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
      if("${CMAKE_GENERATOR}" STREQUAL "NMake Makefiles")
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\dsmp.otf" "${CMAKE_CURRENT_BINARY_DIR}\\dsmp.otf" COPYONLY)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\config.txt" COPYONLY)
      else()
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\dsmp.otf" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\dsmp.otf" COPYONLY)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\config.txt" COPYONLY)
      endif()
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MINGW")
      if(LINK_MANDELBROT_STATIC)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\dsmp.otf" "${CMAKE_CURRENT_BINARY_DIR}\\dsmp.otf" COPYONLY)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\config.txt" COPYONLY)
      else()
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\dsmp.otf" "${CMAKE_CURRENT_BINARY_DIR}\\dsmp.otf" COPYONLY)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\config.txt" COPYONLY)
      endif()
    endif()
  else()
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}/dsmp.otf" "${CMAKE_CURRENT_BINARY_DIR}\\dsmp.otf" COPYONLY)
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\config.txt" COPYONLY)
    endif()
  endif()
else()
  if(WIN32)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
      if("${CMAKE_GENERATOR}" STREQUAL "NMake Makefiles")
        configure_file("${SFML_SYSTEM_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-system-2.dll" COPYONLY)
        configure_file("${SFML_WINDOW_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-window-2.dll" COPYONLY)
        configure_file("${SFML_GRAPHICS_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-graphics-2.dll" COPYONLY)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\dsmp.otf" "${CMAKE_CURRENT_BINARY_DIR}\\dsmp.otf" COPYONLY)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\config.txt" COPYONLY)
      else()
        configure_file("${SFML_SYSTEM_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\sfml-system-2.dll" COPYONLY)
        configure_file("${SFML_WINDOW_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\sfml-window-2.dll" COPYONLY)
        configure_file("${SFML_GRAPHICS_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\sfml-graphics-2.dll" COPYONLY)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\dsmp.otf" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\dsmp.otf" COPYONLY)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\Release\\config.txt" COPYONLY)
      endif()
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MINGW")
      configure_file("${SFML_SYSTEM_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-system-2.dll" COPYONLY)
      configure_file("${SFML_WINDOW_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-window-2.dll" COPYONLY)
      configure_file("${SFML_GRAPHICS_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-graphics-2.dll" COPYONLY)
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\dsmp.otf" "${CMAKE_CURRENT_BINARY_DIR}\\dsmp.otf" COPYONLY)
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\config.txt" COPYONLY)
    endif()
  else()
    configure_file("${SFML_SYSTEM_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-system-2.dll" COPYONLY)
    configure_file("${SFML_WINDOW_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-window-2.dll" COPYONLY)
    configure_file("${SFML_GRAPHICS_DLL}" "${CMAKE_CURRENT_BINARY_DIR}\\sfml-graphics-2.dll" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\dsmp.otf" "${CMAKE_CURRENT_BINARY_DIR}\\dsmp.otf" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}\\config.txt" "${CMAKE_CURRENT_BINARY_DIR}\\config.txt" COPYONLY)
  endif()
endif()

# if(WIN32)
# else()
# endif(WIN32)
# Install target
#install(TARGETS Mandelbrot DESTINATION bin)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()