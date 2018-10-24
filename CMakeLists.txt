cmake_minimum_required(VERSION 3.0)

list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/tbb/cmake)

include(ExternalProject)

include("cmake/organize_sources.cmake")
include("cmake/mkl.cmake")

project(rtff)

option(rtff_enable_tests "Build Unit tests" ON)
option(rtff_enable_multithread "Allow multithreading" OFF)
option(rtff_use_mkl "Use the mkl backend to compute faster ffts and matrix operation" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_DEBUG_POSTFIX d)  # add the d postfix to generated libraries

# Intel Mkl
if (${rtff_use_mkl})
  FindMkl()
else ()
  # Default to fftw
  set(fftw_source_dir ${CMAKE_BINARY_DIR}/fftw-source)
  set(fftw_install_dir ${CMAKE_BINARY_DIR}/fftw-install)
  ExternalProject_Add(external_fftw
    URL "http://www.fftw.org/fftw-3.3.5.tar.gz"
    URL_MD5 6cc08a3b9c7ee06fdd5b9eb02e06f569
    SOURCE_DIR ${fftw_source_dir}
    BINARY_DIR ${fftw_source_dir}
    INSTALL_DIR ${fftw_install_dir}
    BUILD_COMMAND "make"
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ./configure --prefix=${fftw_install_dir} --enable-static --enable-float
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
