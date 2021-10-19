# Project metadata
cmake_minimum_required(VERSION 3.12)
project(DivinityMachine)
set(CMAKE_CXX_STANDARD 17)

# Source files
file(GLOB SOURCES
        ./main.cpp
        ./bimage.hpp
        ./dds.hpp
        )

# Compile arguments
if (MSVC)
        add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
        set(CMAKE_CXX_FLAGS "/O2 /Oi /Ot /GL /EHsc")
else()
        set (CMAKE_CXX_FLAGS "-Ofast -DNDEBUG -Wno-unused-result")
endif()

# Add DivinityMachine executable
add_executable(DivinityMachine ${SOURCES})

# use LTCG linking on MSVC
if(MSVC)
        target_link_options(DivinityMachine PUBLIC "/LTCG")
endif()
