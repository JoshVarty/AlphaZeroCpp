cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(dcgan)

find_package(Torch REQUIRED)

add_executable(AlphaZeroCpp src/main.cpp)
target_link_libraries(AlphaZeroCpp "${TORCH_LIBRARIES}")
set_property(TARGET AlphaZeroCpp PROPERTY CXX_STANDARD 14)
