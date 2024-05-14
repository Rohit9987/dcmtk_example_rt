cmake_minimum_required(VERSION 3.0)

project(1)

find_package(ITK REQUIRED)
find_package(DCMTK REQUIRED)

include_directories(${DCMTK_INCLUDE_DIRS})
include(${ITK_USE_FILE})

add_executable(output.out main.cc)

target_link_libraries(output.out ${DCMTK_LIBRARIES} ${ITK_LIBRARIES})
