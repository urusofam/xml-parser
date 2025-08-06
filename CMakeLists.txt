cmake_minimum_required(VERSION 3.21)
project(xml_parser CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(xml_parser main.cpp)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

add_executable(xml_parser_test xml_parser_test.cpp)
target_link_libraries(xml_parser_test PRIVATE gtest)

enable_testing()
add_test(NAME xml_parser COMMAND xml_parser_test)