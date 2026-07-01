cmake_minimum_required(VERSION 3.15)
project(calculator-cli VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/lexer.h
    src/lexer.cpp
    src/parser.h
    src/parser.cpp
)

# Output executable
add_executable(calculator-cli ${SOURCES})

# Target include directories
target_include_directories(calculator-cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Compiler flags for warnings
if(MSVC)
    target_compile_options(calculator-cli PRIVATE /W4 /WX)
else()
    target_compile_options(calculator-cli PRIVATE -Wall -Wextra -Werror)
    target_link_options(calculator-cli PRIVATE -static)
endif()

