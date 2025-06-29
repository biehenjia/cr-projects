cmake_minimum_required(VERSION 3.15)
project(pycr LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch pybind11 via FetchContent
include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(pybind11)

# Collect source files
file(GLOB SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/bindings/*.cpp"
)

# Build the Python extension; name must match PYBIND11_MODULE in code
pybind11_add_module(pycr ${SOURCE_FILES})

# Include directories (headers in include/ and src/)
target_include_directories(pycr
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src"
)

# Link against pybind11 runtime and static runtime libraries on MinGW
if (MINGW)
  # Static link libgcc and libstdc++
  target_link_options(pycr PRIVATE
    -static-libgcc
    -static-libstdc++
  )
  # Statically link winpthread by passing through linker flags
  target_link_libraries(pycr PRIVATE
    pybind11::module
    "-Wl,-Bstatic"
    "-lwinpthread"
    "-Wl,-Bdynamic"
  )
else()
  # Standard dynamic link on other platforms
  target_link_libraries(pycr PRIVATE pybind11::module)
endif()

# Override output name and suffix to avoid version tag
# Place the .pyd into the project root on Windows
if (WIN32)
    set_target_properties(pycr PROPERTIES
        OUTPUT_NAME "pycr"
        PREFIX ""
        SUFFIX ".pyd"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
else()
    # On non-Windows, use standard .so in source dir
    set_target_properties(pycr PROPERTIES
        OUTPUT_NAME "pycr"
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()
