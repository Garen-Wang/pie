cmake_minimum_required(VERSION 3.14)

# add dependency
include(cmake/CPM.cmake)
CPMAddPackage( gh:Garen-Wang/PEGParser@2.2.1 )

# cpp parser
project(cpp_parser)

add_executable(cpp_parser
  src/cpp/main.cpp
    )

target_include_directories(cpp_parser
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)


target_link_libraries(cpp_parser PEGParser::PEGParser)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# shl
project(shl)

add_executable(shl
    src/shl/main.cpp
    include/shl/parser_builder.h include/shl/presets.h src/shl/presets.cpp src/shl/parser_builder.cpp)

target_include_directories(shl
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    )
target_link_libraries(shl PEGParser::PEGParser)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
