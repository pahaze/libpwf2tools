cmake_minimum_required(VERSION 3.5)

project(pwf2log LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(LIBRARY_SOURCES
    libpwf2log.cpp
    ${libpwf2log_SOURCE_DIR}/include/pwf2tools/libpwf2log.h
)

add_library(pwf2log ${LIBRARY_SOURCES})
target_include_directories(pwf2log PRIVATE ${libpwf2log_SOURCE_DIR}/include)
target_link_libraries(pwf2log PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

install(TARGETS pwf2log RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES ${libpwf2log_SOURCE_DIR}/include/pwf2tools/libpwf2log.h DESTINATION include/pwf2tools)
