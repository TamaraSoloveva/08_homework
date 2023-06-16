cmake_minimum_required(VERSION 3.5)

project(08_homework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(08_homework main.cpp )


if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(08_homework PRIVATE -Wall -Wextra -Werror -pedantic)

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(08_homework PRIVATE /EHsc /W4 /WX)
 endif()
