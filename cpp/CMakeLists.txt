cmake_minimum_required(VERSION 3.15)
project(pycr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Release only for single-config generators.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG v3.0.0
)
FetchContent_MakeAvailable(pybind11)

file(GLOB SOURCE_FILES
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/bindings/*.cpp"
)

pybind11_add_module(pycr ${SOURCE_FILES})

target_include_directories(pycr PRIVATE
  "${CMAKE_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/src"
)

# ---- Profiling define ----
option(CREVAL_PROFILE "Enable timers inside _creval()" OFF)
target_compile_definitions(pycr PRIVATE
  $<$<BOOL:${CREVAL_PROFILE}>:CREVAL_PROFILE>
)

# Helper generator expressions
set(IS_GNU_OR_CLANG $<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>)
set(IS_MSVC $<CXX_COMPILER_ID:MSVC>)
set(IS_DEBUG $<CONFIG:Debug>)
set(IS_NOT_DEBUG $<NOT:${IS_DEBUG}>)

# ---- Optimizations per compiler/config ----
if(MSVC)
  target_compile_options(pycr PRIVATE
    $<${IS_NOT_DEBUG}:/O2 /GL /fp:fast /GS- /Zi>
    $<${IS_DEBUG}:/Od /Z7>
  )
  target_link_options(pycr PRIVATE
    $<${IS_NOT_DEBUG}:/LTCG>
  )
else()
  # Non-MSVC: GCC/Clang
  target_compile_options(pycr PRIVATE
    $<${IS_DEBUG}:$<${IS_GNU_OR_CLANG}:-O0 -g>>
    $<${IS_NOT_DEBUG}:$<${IS_GNU_OR_CLANG}:-O3 -fno-math-errno -fno-trapping-math -ffp-contract=fast -fstrict-aliasing>>
    # GCC-only extras (guard -fno-plt)
    $<$<AND:${IS_NOT_DEBUG},$<CXX_COMPILER_ID:GNU>>:-fno-plt>
    # Optional: enable if you are okay with host-specific binaries (not for manylinux wheels)
    # $<$<AND:${IS_NOT_DEBUG},${IS_GNU_OR_CLANG}>:-march=native>
  )
  # Linker tidying for ELF platforms (harmless elsewhere)
  target_link_options(pycr PRIVATE
    $<$<AND:${IS_NOT_DEBUG},${IS_GNU_OR_CLANG}>:-Wl,--as-needed>
  )

  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_supported OUTPUT ipo_msg)
  if(ipo_supported)
    # Apply IPO to non-Debug configs
    set_property(TARGET pycr PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    set_property(TARGET pycr PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
  endif()
endif()

# ---- MinGW static bits (kept) ----
if(MINGW)
  target_link_options(pycr PRIVATE -static-libgcc -static-libstdc++)
  target_link_libraries(pycr PRIVATE pybind11::module "-Wl,-Bstatic" "-lwinpthread" "-Wl,-Bdynamic")
else()
  target_link_libraries(pycr PRIVATE pybind11::module)
endif()

# ---- Output: let pybind11 manage suffix/name ----
# If you really need a custom name (not recommended), do OUTPUT_NAME only:
set_target_properties(pycr PROPERTIES OUTPUT_NAME "pycrlib")
