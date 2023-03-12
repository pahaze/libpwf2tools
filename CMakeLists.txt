cmake_minimum_required(VERSION 3.5)

project(libpwf2tools LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(BUILD_LIBISOMOD "Build libisomod" ON)

if(BUILD_LIBISOMOD)
    add_subdirectory(libisomod)
endif(BUILD_LIBISOMOD)
