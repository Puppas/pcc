cmake_minimum_required(VERSION 3.2)
project(pcc)

aux_source_directory(${CMAKE_SOURCE_DIR} src_list)
add_executable(pcc ${src_list})