cmake_minimum_required(VERSION 3.0)

list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/tbb/cmake)

include(ExternalProject)

include("cmake/organize_sources.cmake")
include("cmake/mkl.cmake")

project(rtff)

option(rtff_enable_tests "Build Unit tests" ON)
option(rtff_enable_multithread "Allow multithreading" OFF)
option(rtff_use_mkl "Use the mkl backend to compute faster ffts and matrix operation" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_DEBUG_POSTFIX d)  # add the d postfix to generated libraries

if (${rtff_use_mkl})
  # Intel Mkl
  FindMkl()
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
  set(tbb_install_dir ${CMAKE_BINARY_DIR}/tbb-install)
  ExternalProject_Add(external_tbb
    GIT_REPOSITORY "https://github.com/wjakob/tbb.git"
    GIT_TAG "tbb44u4"
    # to avoid long paths on windows, build in the binary folder
    SOURCE_DIR ${CMAKE_BINARY_DIR}/tbb-sources
    BUILD_IN_SOURCE 1
    CMAKE_ARGS
      "-DTBB_BUILD_STATIC=OFF"
      "-DTBB_BUILD_TBBMALLOC=OFF"
      "-DTBB_BUILD_TBBMALLOC_PROXY=OFF"
      "-DTBB_BUILD_TESTS=OFF"
      "-DCMAKE_INSTALL_PREFIX=${tbb_install_dir}"
  )
  link_directories(${tbb_install_dir}/lib)
  if (WIN32)
    install(DIRECTORY
      ${tbb_install_dir}/bin
      DESTINATION .
    )
  endif()
  install(DIRECTORY
    ${tbb_install_dir}/lib
    DESTINATION .
  )
endif()

if (${rtff_enable_tests})
  enable_testing()

  # gtest
  set(gtest_install_dir ${CMAKE_BINARY_DIR}/googletest-install)
  ExternalProject_Add(external_googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG "718fd88d8f145c63b8cc134cf8fed92743cc112f"
    CMAKE_ARGS
      "-Dgtest_force_shared_crt=ON"
      "-DCMAKE_INSTALL_PREFIX=${gtest_install_dir}"
  )
  link_directories("${gtest_install_dir}/lib")

  # download test resource from https://archive.org/details/test_wav
  set(test_resource_path "${CMAKE_BINARY_DIR}/test_resource")
  file(MAKE_DIRECTORY "${test_resource_path}")
  file(DOWNLOAD
    "https://archive.org/download/test_wav/Untitled3.wav"
    "${test_resource_path}/Untitled3.wav"
  )
endif ()

set(src "${CMAKE_SOURCE_DIR}/src")
include_directories(${src})

add_subdirectory("./src")
