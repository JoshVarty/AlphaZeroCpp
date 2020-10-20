cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(AlphaZeroCpp)

find_package(Torch REQUIRED)

add_executable(AlphaZeroCpp main.cpp)
target_link_libraries(AlphaZeroCpp "${TORCH_LIBRARIES}")
set_property(TARGET AlphaZeroCpp PROPERTY CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# Find all relevant files in /src
include_directories(${CMAKE_SOURCE_DIR}/src)
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

foreach(dir ${dirs})
    file(GLOB_RECURSE SOURCE ${dir}/*.[ch]*)
    set(SOURCE_FILES "${SOURCE_FILES}" ${SOURCE})
endforeach()

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
# Link runTests with what we want to test and the GTest and pthread library
set(SOURCE_FILES "${SOURCE_FILES}" test/test_runner.cpp)
add_executable(runTests ${SOURCE_FILES})

target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)
target_link_libraries(runTests "${TORCH_LIBRARIES}")


