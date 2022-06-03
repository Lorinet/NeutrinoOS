cmake_minimum_required (VERSION 3.8)
project(bootsplash)

add_executable(bootsplash "main.c")

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(bootsplash ${PNG_LIBRARY})

