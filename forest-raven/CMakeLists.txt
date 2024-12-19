cmake_minimum_required(VERSION 3.0)
 
project(
    Forest-Raven
    VERSION 1.0.0  # 프로젝트 버전
    DESCRIPTION "Forest-Raven" # 프로젝트 설명
    LANGUAGES CXX # 사용할 언어
)
 
set(CMAKE_CXX_STANDARD 20)

add_executable(forest-raven ForestRaven.cpp)