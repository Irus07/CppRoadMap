cmake_minimum_required(VERSION 3.20)
project(MinesweeperGame VERSION 1.0.0 LANGUAGES CXX)

# Включаем современный стандарт C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Создаем исполняемый файл из всех .cpp файлов
add_executable(Minesweeper
    src/main.cpp
    src/Cell.cpp
    src/Board.cpp
    src/Game.cpp
)

# Указываем путь к папке с заголовочными файлами .hpp
target_include_directories(Minesweeper PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Строгие флаги компиляции
if (MSVC)
    target_compile_options(Minesweeper PRIVATE /W4 /permissive-)
else()
    target_compile_options(Minesweeper PRIVATE -Wall -Wextra -Wpedantic)
endif()
