cmake_minimum_required(VERSION 3.0)

project(1)

find_package(DCMTK REQUIRED)

include_directories(${DCMTK_INCLUDE_DIRS})

add_executable(output.out main.cc)

target_link_libraries(output.out ${DCMTK_LIBRARIES})
