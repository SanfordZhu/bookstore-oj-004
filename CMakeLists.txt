cmake_minimum_required(VERSION 3.10)
project(BookstoreManagement)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Source files
set(SOURCES
    main.cpp
    user.cpp
    book.cpp
    log.cpp
    utils.cpp
)

# Header files
set(HEADERS
    user.hpp
    book.hpp
    log.hpp
    utils.hpp
)

# Create executable
add_executable(code ${SOURCES} ${HEADERS})

# Compiler flags for optimization
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(code PRIVATE -O2 -Wall -Wextra)
endif()