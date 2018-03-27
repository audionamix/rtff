cmake_minimum_required(VERSION 3.0)

list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/tbb/cmake)

include(ExternalProject)

include("cmake/organize_sources.cmake")
include("cmake/mkl.cmake")

project(rtff)

option(rtff_enable_tests "Build Unit tests" ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_DEBUG_POSTFIX d)  # add the d postfix to generated libraries

# Intel TBB
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

# Intel Mkl
FindMkl(TBB_PATH ${tbb_install_dir}/bin)

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
