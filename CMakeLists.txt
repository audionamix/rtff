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
else ()
  # Default to fftw
  if (WIN32)
    message(FATAL_ERROR "The fftw backend is not supported on Windows. Please use the mkl.")
  endif ()
  option(rtff_fftw_use_wisdom "Use the fftw wisdom mechanism. WARNING: first computation may take up to a couple of minutes" OFF)
  set(rtff_fftw_extra_configure_flags "" CACHE STRING "Extra flags used in fftw configure step")

  set(fftw_source_dir ${CMAKE_BINARY_DIR}/fftw-source)
  set(fftw_install_dir ${CMAKE_BINARY_DIR}/fftw-install)

  # We add the project CXX and C flags to the configure command
  set(fftw_configure_command ./configure --prefix=${fftw_install_dir} --enable-static --enable-float)
  if (CMAKE_CXX_FLAGS)
    set(fftw_configure_command ${fftw_configure_command} CPPFLAGS=${CMAKE_CXX_FLAGS})
  endif()
  if (CMAKE_C_FLAGS)
    set(fftw_configure_command ${fftw_configure_command} CFLAGS=${CMAKE_C_FLAGS})
  endif()
  if (rtff_fftw_extra_configure_flags)
    set(fftw_configure_command ${fftw_configure_command} ${rtff_fftw_extra_configure_flags})
  endif()

  ExternalProject_Add(external_fftw
    URL "http://www.fftw.org/fftw-3.3.7.tar.gz"
    URL_MD5 0d5915d7d39b3253c1cc05030d79ac47
    SOURCE_DIR ${fftw_source_dir}
    BINARY_DIR ${fftw_source_dir}
    INSTALL_DIR ${fftw_install_dir}
    BUILD_COMMAND "make"
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ${fftw_configure_command}
  )
  link_directories(${fftw_install_dir}/lib)
  set(fftw_include_dir ${fftw_install_dir}/include)
  set(fftw_libraries fftw3f)
endif()

# Intel TBB
if (${rtff_enable_multithread})
  include(add_tbb)
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
