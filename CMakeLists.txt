cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add dependency
include(cmake/CPM.cmake)
CPMAddPackage(gh:Garen-Wang/PEGParser@2.2.7)

project(pie VERSION 0.1 LANGUAGES CXX)

# shl: syntax highlighting language
add_library(
  shl
  ${PROJECT_SOURCE_DIR}/src/shl/parser_builder.cpp
  ${PROJECT_SOURCE_DIR}/src/shl/presets.cpp
)
target_include_directories(
  shl
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include/shl
  ${PROJECT_SOURCE_DIR}/include/ui
  INTERFACE
  ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(shl PEGParser::PEGParser Qt5::Core Qt5::Widgets)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)

add_library(
  pierc
  ${PROJECT_SOURCE_DIR}/src/pierc/pierc.cpp
)
target_include_directories(
  pierc
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include/ui
  ${PROJECT_SOURCE_DIR}/include/pierc
  INTERFACE
  ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(pierc PEGParser::PEGParser shl Qt5::Core Qt5::Widgets)

# pie: Pie Is an Editor
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
  ${PROJECT_SOURCE_DIR}/include/pierc
  ${PROJECT_SOURCE_DIR}/include/ui
  INTERFACE
  ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(pie Qt5::Core Qt5::Widgets shl pierc)
