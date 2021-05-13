cmake_minimum_required(VERSION 3.14)

# add dependency
include(cmake/CPM.cmake)
CPMAddPackage(
  NAME PEGParser
  VERSION 2.1.1
  GITHUB_REPOSITORY TheLartians/PEGParser
)

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

# python parser
project(python_poc)

add_executable(python_poc
  src/python/poc.cpp
)

target_include_directories(python_poc
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(python_poc PEGParser::PEGParser)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# python parser
project(python_parser)

add_executable(python_parser
  src/python/main.cpp
)

target_include_directories(python_parser
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(python_parser PEGParser::PEGParser)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)