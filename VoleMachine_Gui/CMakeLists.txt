cmake_minimum_required(VERSION 3.16)
project(VoleGui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# every .cpp in this folder (simulator + Qt files), except the console / web mains
file(GLOB SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(FILTER SOURCES EXCLUDE REGEX ".*/(main|gui_main|VoleMachineSim)\\.cpp$")

add_executable(VoleGui WIN32 ${SOURCES})
target_link_libraries(VoleGui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
