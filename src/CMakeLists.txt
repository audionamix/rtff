# Eigen
set(eigen_install_dir ${CMAKE_BINARY_DIR}/eigen-install)
ExternalProject_Add(external_eigen
  GIT_REPOSITORY "https://github.com/eigenteam/eigen-git-mirror.git"
  GIT_TAG "3.3.6"
  SOURCE_DIR ${eigen_install_dir}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ""
)
set(eigen_include_dir ${eigen_install_dir})
# make sure that no malloc are made in the run loop
add_definitions("-DEIGEN_RUNTIME_NO_MALLOC")

add_subdirectory("${src}/rtff")
