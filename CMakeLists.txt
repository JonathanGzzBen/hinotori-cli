cmake_minimum_required(VERSION 3.14)

project(hinotori-cli LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/hinotori-core)
include_directories(libs/hinotori-core/include)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_executable(hinotori-cli
  src/main.cpp
)
target_link_libraries(hinotori-cli hinotori-core Qt${QT_VERSION_MAJOR}::Core)
