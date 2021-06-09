cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(pie VERSION 0.1 LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)


# add dependency
include(cmake/CPM.cmake)
# CPMAddPackage(gh:Garen-Wang/PEGParser@2.2.2)
CPMAddPackage(
    NAME PackageProject.cmake
    GITHUB_REPOSITORY TheLartians/PackageProject.cmake
    VERSION 1.4
)
CPMAddPackage(
    NAME EasyIterator
    GITHUB_REPOSITORY TheLartians/EasyIterator
    VERSION 1.4
)

# PEGParser
add_library(
    PEGParser
    ${PROJECT_SOURCE_DIR}/src/peg_parser/presets.cpp
    ${PROJECT_SOURCE_DIR}/src/peg_parser/interpreter.cpp
    ${PROJECT_SOURCE_DIR}/src/peg_parser/parser.cpp
    ${PROJECT_SOURCE_DIR}/src/peg_parser/presets.cpp
)
target_include_directories(
    PEGParser
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include/peg_parser
    INTERFACE
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(PEGParser EasyIterator Qt5::Core Qt5::Widgets)

# shl
add_library(
    shl
    ${PROJECT_SOURCE_DIR}/src/shl/parser_builder.cpp
    ${PROJECT_SOURCE_DIR}/src/shl/presets.cpp
)
target_include_directories(
    shl
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include/shl
    INTERFACE
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(shl PEGParser)


qt5_wrap_cpp(window_src ${PROJECT_SOURCE_DIR}/include/ui/window.hpp)
qt5_wrap_cpp(textArea_src ${PROJECT_SOURCE_DIR}/include/ui/textArea.hpp)

add_executable(pie
  ${PROJECT_SOURCE_DIR}/src/ui/config.cpp
  ${PROJECT_SOURCE_DIR}/src/ui/rope.cpp
  ${PROJECT_SOURCE_DIR}/src/ui/actions.cpp
  ${PROJECT_SOURCE_DIR}/src/ui/modes.cpp

  ${window_src}
  ${textArea_src}
  ${PROJECT_SOURCE_DIR}/src/ui/textArea.cpp
  ${PROJECT_SOURCE_DIR}/src/ui/window.cpp

  ${PROJECT_SOURCE_DIR}/src/ui/main.cpp
)
include_directories(
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include/ui
    INTERFACE
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(pie shl)
