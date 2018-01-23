# Eigen
set(eigen_install_dir ${CMAKE_BINARY_DIR}/eigen-install)
ExternalProject_Add(external_eigen
  HG_REPOSITORY "https://bitbucket.org/eigen/eigen/"
  HG_TAG "3.3.4"
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
