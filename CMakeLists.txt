cmake_minimum_required(VERSION 3.0)

project(1)

find_package(ITK REQUIRED)
find_package(DCMTK REQUIRED)

include_directories(${DCMTK_INCLUDE_DIRS})
include(${ITK_USE_FILE})

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")
file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/*.h")

add_executable(output.out main.cc ${HEADERS} ${SOURCES})

target_link_libraries(output.out ${DCMTK_LIBRARIES} ${ITK_LIBRARIES})
