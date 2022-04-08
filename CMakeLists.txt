cmake_minimum_required(VERSION 3.1)

# Set project name and version
project(Mandelbrot)

#set(CMAKE_VERBOSE_MAKEFILE ON)
MESSAGE(NOTICE "Before generating and building the project, please ensure that it is configured correctly. Ignore this message if done already.")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)")
MESSAGE(STATUS "Building is forced to be in ${CMAKE_BUILD_TYPE} mode...")
set(PROJECT_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CUSTOM_LIB_DIR "" CACHE PATH "NOTICE: If you built the SFML library and its dependencies in a different location, you can specify its directory here") #Custom Libraries location
set(PROJECT_ARCH "x86" CACHE STRING "Choose the type of architecture (x86 or x64)") # x86 or x64 only
set(LINK_MANDELBROT_STATIC OFF CACHE BOOL "Link SFML library statically?")

# Set CXX_STANDARD
if(WIN32)
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MINGW" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    MESSAGE(STATUS "MinGW Toolchain detected")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(MSVC_PATCH WIN32)
    set ( CXX_STANDARD 11 )
    MESSAGE(STATUS "MSVC Toolchain detected")
  endif()
else()
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    MESSAGE(STATUS "GNU Toolchain detected")
  else()
    set(CXX_STANDARD 11)
  endif()
endif(WIN32)

# SFML version
set(SFML_VERSION_AB 2.5)
set(SFML_VERSION_ABX 2.5.1)

# Executables to be compiled
add_executable(Mandelbrot ${MSVC_PATCH}  src/main.cpp src/TinyBitmapOut.cpp)
add_executable(Test ${MSVC_PATCH} test/test.cpp)

# Set include directory search paths
target_include_directories(Mandelbrot
  PRIVATE "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" "include" "SFML\\include" "SFML/include"
)
target_include_directories(Test
  PRIVATE "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" "include" "SFML\\include" "SFML/include"
)

if(WIN32)
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MINGW" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(FREETYPE_LIB_NAME "libfreetype.a")
    set(SFML_LIB_DIR "SFML/build/lib")
    set(SFML_EXT_LIB "SFML/extlibs/libs-mingw/${PROJECT_ARCH}")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(FREETYPE_LIB_NAME "freetype.lib")
    set(SFML_LIB_DIR "SFML\\build\\lib\\${PROJECT_BUILD_TYPE}")
    set(SFML_EXT_LIB "SFML\\extlibs\\libs-msvc\\${PROJECT_ARCH}")
  else()
    message(FATAL_ERROR "Static library for the current compiler is not yet supported...")
  endif()
else()
  set(FREETYPE_LIB_NAME "libfreetype.a")
  set(SFML_LIB_DIR "SFML/build/lib")
  set(SFML_EXT_LIB "")
endif()

if(LINK_MANDELBROT_STATIC)
  MESSAGE(STATUS "Finding static libraries")
  find_library(FREETYPE_LIB
  NAMES "${FREETYPE_LIB_NAME}"
  HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}" "${SFML_EXT_LIB}"
  )

  find_library(SFML_GRAPHICS_LIB
  NAMES "libsfml-graphics-s.a" "sfml-graphics-s.lib"
  HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
  )

  find_library(SFML_WINDOW_LIB
  NAMES "libsfml-window-s.a" "sfml-window-s.lib"
  HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
  )

  find_library(SFML_SYSTEM_LIB
  NAMES "libsfml-system-s.a" "sfml-system-s.lib"
  HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
  )

else()
  find_library(SFML_GRAPHICS_LIB
  NAMES sfml-graphics
  HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
  )

  find_library(SFML_WINDOW_LIB
  NAMES sfml-window
  HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
  )

  find_library(SFML_SYSTEM_LIB
  NAMES sfml-system
  HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
  )

  # shared libraries
  find_file(SFML_GRAPHICS_DLL
  NAMES "sfml-graphics-2.dll" "libsfml-graphics.so"
  HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
  )

  find_file(SFML_WINDOW_DLL
  NAMES "sfml-window-2.dll" "libsfml-window.so"
  HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
  )

  find_file(SFML_SYSTEM_DLL
  NAMES "sfml-system-2.dll" "libsfml-system.so"
  HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
  )

  if(UNIX)
    find_file(SFML_GRAPHICS_SO
    NAMES "libsfml-graphics.so.${SFML_VERSION_AB}"
    HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
    )

    find_file(SFML_WINDOW_SO
    NAMES "libsfml-window.so.${SFML_VERSION_AB}"
    HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
    )

    find_file(SFML_SYSTEM_SO
    NAMES "libsfml-system.so.${SFML_VERSION_AB}"
    HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
    )

    find_file(SFML_GRAPHICS_MAIN
    NAMES "libsfml-graphics.so.${SFML_VERSION_ABX}"
    HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
    )
  
    find_file(SFML_WINDOW_MAIN
    NAMES "libsfml-window.so.${SFML_VERSION_ABX}"
    HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
    )
  
    find_file(SFML_SYSTEM_MAIN
    NAMES "libsfml-system.so.${SFML_VERSION_ABX}"
    HINTS "${CUSTOM_LIB_DIR}" "lib" "${SFML_LIB_DIR}"
    )
  message(STATUS "The linux shared libraries are ${SFML_GRAPHICS_SO} => ${SFML_GRAPHICS_MAIN}, ${SFML_WINDOW_SO} => ${SFML_WINDOW_MAIN}, ${SFML_SYSTEM_SO} => ${SFML_SYSTEM_MAIN}")
  endif()
endif()

# Linking
if(LINK_MANDELBROT_STATIC)
  add_compile_definitions(SFML_STATIC)
  if(WIN32)
    MESSAGE(STATUS "The static libraries to be linked at are ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} ${FREETYPE_LIB} opengl32 winmm gdi32")
    target_link_libraries(Mandelbrot ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} ${FREETYPE_LIB} opengl32 winmm gdi32)
    target_link_libraries(Test ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} ${FREETYPE_LIB} opengl32 winmm gdi32)
  else()
    MESSAGE(STATUS "The static libraries to be linked at are freetype GL udev sfml-graphics-s sfml-window-s sfml-system-s X11 pthread")
    target_link_libraries(Mandelbrot freetype GL udev ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} X11 pthread)
    target_link_libraries(Test freetype GL udev ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} X11 pthread)
  endif()
else()
  message(STATUS "he shared libraries are ${SFML_GRAPHICS_DLL} ${SFML_WINDOW_DLL} ${SFML_SYSTEM_DLL}")
  if(WIN32)
    target_link_libraries(Mandelbrot ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB})
    target_link_libraries(Test ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB})
  else()
    target_link_libraries(Mandelbrot X11 pthread ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB})
    target_link_libraries(Test X11 pthread ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB})
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
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}/dsmp.otf" "${CMAKE_CURRENT_BINARY_DIR}/dsmp.otf" COPYONLY)
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.txt" "${CMAKE_CURRENT_BINARY_DIR}/config.txt" COPYONLY)
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
    configure_file("${SFML_SYSTEM_DLL}" "${CMAKE_CURRENT_BINARY_DIR}/libsfml-system.so" COPYONLY)
    configure_file("${SFML_WINDOW_DLL}" "${CMAKE_CURRENT_BINARY_DIR}/libsfml-window.so" COPYONLY)
    configure_file("${SFML_GRAPHICS_DLL}" "${CMAKE_CURRENT_BINARY_DIR}/libsfml-graphics.so" COPYONLY)
    configure_file("${SFML_SYSTEM_SO}" "${CMAKE_CURRENT_BINARY_DIR}/libsfml-system.so.${SFML_VERSION_AB}" COPYONLY)
    configure_file("${SFML_WINDOW_SO}" "${CMAKE_CURRENT_BINARY_DIR}/libsfml-window.so.${SFML_VERSION_AB}" COPYONLY)
    configure_file("${SFML_GRAPHICS_SO}" "${CMAKE_CURRENT_BINARY_DIR}/libsfml-graphics.so.${SFML_VERSION_AB}" COPYONLY)
    configure_file("${SFML_SYSTEM_MAIN}" "${CMAKE_CURRENT_BINARY_DIR}/libsfml-system.so.${SFML_VERSION_ABX}" COPYONLY)
    configure_file("${SFML_WINDOW_MAIN}" "${CMAKE_CURRENT_BINARY_DIR}/libsfml-window.so.${SFML_VERSION_ABX}" COPYONLY)
    configure_file("${SFML_GRAPHICS_MAIN}" "${CMAKE_CURRENT_BINARY_DIR}/libsfml-graphics.so.${SFML_VERSION_ABX}" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/dsmp.otf" "${CMAKE_CURRENT_BINARY_DIR}/dsmp.otf" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.txt" "${CMAKE_CURRENT_BINARY_DIR}/config.txt" COPYONLY)
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