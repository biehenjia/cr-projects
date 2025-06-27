cmake_minimum_required(VERSION 3.15)
project(crast LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG "master"
)
FetchContent_MakeAvailable(pybind11)

set(CR_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)


set(SOURCES
  src/_ast.cpp
  src/crexpr.cpp
  src/crsum.cpp
  src/crprod.cpp
  src/crnum.cpp
  src/crobj.cpp
  bindings/bindings.cpp  
)


pybind11_add_module(crast ${SOURCES})

target_include_directories(crast
  PRIVATE
    ${CR_INCLUDE_DIR}
)

