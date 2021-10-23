cmake_minimum_required (VERSION 3.8)
project(NeutrinoOS)
add_subdirectory(NeutrinoOS)

install(TARGETS NeutrinoOS DESTINATION "bin")
