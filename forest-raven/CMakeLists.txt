cmake_minimum_required(VERSION 3.30)

project(
    Forest_Raven
    VERSION 0.1
    DESCRIPTION "chess"
    LANGUAGES CXX)
  
project(${PROJECT_NAME})
add_executable(${PROJECT_NAME}
    ForestRaven.cpp
    utility.h board.h
    )