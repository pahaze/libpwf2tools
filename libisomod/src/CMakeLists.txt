cmake_minimum_required(VERSION 3.5)

project(isomod LANGUAGES CXX)

if(WIN32)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-ms-bitfields")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-ms-bitfields")
endif()

set(LIBRARY_SOURCES
	libisomod.cpp
	${libisomod_SOURCE_DIR}/include/pwf2tools/libisomod.h
)

add_library(isomod ${LIBRARY_SOURCES})
target_include_directories(isomod PRIVATE ${libisomod_SOURCE_DIR}/include)

install(TARGETS isomod RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES ${libisomod_SOURCE_DIR}/include/pwf2tools/libisomod.h DESTINATION include/pwf2tools)
