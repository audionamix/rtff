include(FetchContent)

FetchContent_Declare(tbb
  GIT_REPOSITORY https://github.com/wjakob/tbb.git
  GIT_TAG tbb44u4
)

FetchContent_GetProperties(tbb)
if(NOT tbb_POPULATED)
  FetchContent_Populate(tbb)
  set(TBB_BUILD_STATIC OFF CACHE BOOL "")
  set(TBB_BUILD_TBBMALLOC OFF CACHE BOOL "")
  set(TBB_BUILD_TBBMALLOC_PROXY OFF CACHE BOOL "")
  set(TBB_BUILD_TESTS OFF CACHE BOOL "")
  include_directories(${tbb_BINARY_DIR})  # required for version_string.ver
  add_subdirectory(${tbb_SOURCE_DIR} ${tbb_BINARY_DIR})
endif()
