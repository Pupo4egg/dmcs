cmake_minimum_required(VERSION 3.10) # Или более поздняя версия
project(BigFiniteArithmetic)

set(CMAKE_CXX_STANDARD 17) # Или более поздний стандарт C++

# Include директории
include_directories(include)

# Исходные файлы
set(SOURCE_FILES
    src/main.cpp
    src/BigFiniteNumber.cpp
    src/arithmetic.cpp
    src/utils.cpp
    src/constants.cpp
)

# Создание исполняемого файла
add_executable(BigFiniteArithmetic ${SOURCE_FILES})