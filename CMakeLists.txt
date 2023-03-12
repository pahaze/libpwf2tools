cmake_minimum_required(VERSION 3.5)

project(libpwf2tools LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(BUILD_LIBISOMOD "Build libisomod" ON)

if(BUILD_LIBISOMOD)
	add_subdirectory(libisomod)
endif(BUILD_LIBISOMOD)

option(BUILD_LIBPWF2LOG "Build libpwf2log" ON)

if(BUILD_LIBPWF2LOG)
	add_subdirectory(libpwf2log)
endif(BUILD_LIBPWF2LOG)

# Using https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake
if(NOT TARGET uninstall)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
	add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
