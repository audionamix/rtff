cmake_minimum_required(VERSION 3.0)

include(ExternalProject)
include("cmake/mkl.cmake")
include("cmake/organize_sources.cmake")

project(rtff)

option(rtff_enable_tests "Build Unit tests" ON)
set(CMAKE_CXX_STANDARD 14)

# Mkl
FindMkl()

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
