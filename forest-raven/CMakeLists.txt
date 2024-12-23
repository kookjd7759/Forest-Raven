cmake_minimum_required(VERSION 3.0)
 
project(
    Forest-Raven
    VERSION 1.0.0 
    DESCRIPTION "Forest-Raven"
    LANGUAGES CXX 
)
 
set(CMAKE_CXX_STANDARD 20)

add_executable(forest-raven ForestRaven.cpp notation.cpp)