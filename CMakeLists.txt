cmake_minimum_required(VERSION 3.16)
project(sudoku LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Release por defecto: perfilar en debug da resultados engañosos
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# benchmark.cpp incluye src/Board.cpp y src/Solver.cpp directamente,
# así que no se listan aquí (si no, símbolos duplicados al enlazar)
add_executable(benchmark tests/benchmark.cpp)

# Raíz de sudoku/ para que #include "src/Board.cpp" se resuelva
target_include_directories(benchmark PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    target_compile_options(benchmark PRIVATE /O2 /W4 /arch:AVX2)
else()
    target_compile_options(benchmark PRIVATE
        -O3 -march=native -Wall -Wextra
        -fno-omit-frame-pointer   # útil para perf/gprof; quítalo si no perfilas
    )
endif()