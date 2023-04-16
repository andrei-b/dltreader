cmake_minimum_required(VERSION 3.5)

project(dltreader LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB all_SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/dltbase/dlt_common.c" "${PROJECT_SOURCE_DIR}/include/*.h" "${PROJECT_SOURCE_DIR}/src/dltbase/*.h")
add_library(dltreader STATIC ${all_SRCS})
target_include_directories(dltreader PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src/dltbase)

install(TARGETS dltreader
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

add_executable(catdlt examples/catdlt.cpp)
target_include_directories(catdlt PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src/dltbase)
target_link_libraries(catdlt dltreader)
