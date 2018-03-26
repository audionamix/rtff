cmake_minimum_required(VERSION 3.0)

list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/tbb/cmake)

include(ExternalProject)

include("cmake/mkl.cmake")
include("cmake/organize_sources.cmake")
include("cmake/tbb/TBBGet.cmake")
include("cmake/tbb/TBBBuild.cmake")

project(rtff)

option(rtff_enable_tests "Build Unit tests" ON)
set(CMAKE_CXX_STANDARD 14)

# Intel Mkl
FindMkl(TBB_PATH ${tbb_library_dir})

if (${rtff_enable_tests})
  # Intel Tbb as a shared library. Not necessary unless we actually go to link
  # stage which is only necessary when building a shared lib or an executable
  # like the tests.
  # External libs using rtff will need to link against tbb themself
  tbb_get(TBB_ROOT tbb_root RELEASE_TAG "2018_U2" SOURCE_CODE)
  tbb_build(TBB_ROOT ${tbb_root} CONFIG_DIR TBB_DIR)
  set(tbb_library_dir
    ${CMAKE_BINARY_DIR}/tbb_cmake_build/tbb_cmake_build_subdir_release
  )
  link_directories(${tbb_library_dir})

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
