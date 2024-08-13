cmake_minimum_required(VERSION 3.30)

project(
    game
    VERSION 0.1
    DESCRIPTION "chess"
    LANGUAGES CXX)
  
project(${PROJECT_NAME})
add_executable(${PROJECT_NAME}
    game.cpp
    utility.cpp utility.h
    )