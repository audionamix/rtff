cmake_minimum_required(VERSION 3.11)

include(ExternalProject)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(organize_sources)

project(rtff)

option(rtff_enable_tests "Build Unit tests" ON)
option(rtff_enable_multithread "Allow multithreading" OFF)
option(rtff_use_mkl "Use the mkl backend to compute faster ffts and matrix operation" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_DEBUG_POSTFIX d)  # add the d postfix to generated libraries

include(add_eigen)
if (${rtff_use_mkl})
  include(add_mkl)
  set(external_libraries mkl)
else ()
  option(rtff_fftw_use_wisdom "Use the fftw wisdom mechanism. WARNING: first computation may take up to a couple of minutes" OFF)
  set(rtff_fftw_extra_configure_flags "" CACHE STRING "Extra flags used in fftw configure step")
  include(add_fftw)
  set(external_libraries fftw)
endif()

# Intel TBB
if (${rtff_enable_multithread})
  include(add_tbb)
  set(external_libraries ${external_libraries} tbb)
endif()

if (${rtff_enable_tests})
  enable_testing()
  include(add_wave)
  include(add_googletest)

  # download test resource from https://archive.org/details/test_wav
  set(test_resource_path "${CMAKE_CURRENT_BINARY_DIR}/test_resource")
  file(MAKE_DIRECTORY "${test_resource_path}")
  file(DOWNLOAD
    "https://archive.org/download/test_wav/Untitled3.wav"
    "${test_resource_path}/Untitled3.wav"
  )
endif ()

set(src "${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories(${src})

add_subdirectory("./src")
