# Cmake 3.25+ has C++ module support
cmake_minimum_required(VERSION 3.25)

project(mutils)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
add_compile_options(-fdiagnostics-color=always)



if(DEBUG)
    add_compile_options(-Ddbg=1)
    add_compile_options(-Og -g)
else()
    add_compile_options(-O3)
endif()

file(GLOB_RECURSE sources src/*.cc src/*.cpp include/*.h)

if(DEV)
    file(GLOB_RECURSE dev_sources dev/*.cc)
    add_executable(mutils ${sources} ${dev_sources})
else()
    add_library(mutils STATIC ${sources})
endif()
