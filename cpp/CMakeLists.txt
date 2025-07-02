cmake_minimum_required(VERSION 3.15)
project(pycr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")


include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(pybind11)

file(GLOB SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/bindings/*.cpp"
)

pybind11_add_module(pycr ${SOURCE_FILES})

target_include_directories(pycr
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src"
)

if (MINGW)
  target_link_options(pycr PRIVATE
    -static-libgcc
    -static-libstdc++
  )
  target_link_libraries(pycr PRIVATE
    pybind11::module
    "-Wl,-Bstatic"
    "-lwinpthread"
    "-Wl,-Bdynamic"
  )
else()
  target_link_libraries(pycr PRIVATE pybind11::module)
endif()

if (WIN32)
    set_target_properties(pycr PROPERTIES
        OUTPUT_NAME "pycrlib"
        PREFIX ""
        SUFFIX ".pyd"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
else()
    # On non-Windows, use standard .so in source dir
    set_target_properties(pycr PROPERTIES
        OUTPUT_NAME "pycrlib"
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()