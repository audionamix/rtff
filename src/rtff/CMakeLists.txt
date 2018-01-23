add_library(rtff
  ${src}/rtff/filter.cc
  ${src}/rtff/filter.h

  ${src}/rtff/buffer/ring_buffer.cc
  ${src}/rtff/buffer/ring_buffer.h
  ${src}/rtff/buffer/audio_buffer.cc
  ${src}/rtff/buffer/audio_buffer.h
  ${src}/rtff/buffer/buffer.h

  ${src}/rtff/fft/window.h
  ${src}/rtff/fft/window.cc
  ${src}/rtff/fft/fft.h
  ${src}/rtff/fft/fft.cc
  ${src}/rtff/fft/mkl/mkl_fft.h
  ${src}/rtff/fft/mkl/mkl_fft.cc
  ${src}/rtff/fft/mkl/mkl_fft_context.h
  ${src}/rtff/fft/mkl/mkl_fft_context.cc
)
target_include_directories(rtff
  PUBLIC
    ${eigen_install_dir}
    ${mkl_include_dir}
)
add_dependencies(rtff
  external_eigen
)

if (${rtff_enable_tests})
  # libwave
  set(wave_install_dir ${CMAKE_BINARY_DIR}/wave-install)
  ExternalProject_Add(external_wave
    GIT_REPOSITORY "https://github.com/audionamix/wave"
    GIT_TAG "v0.6a"
    # don't update so that we don't rebuild every time
    UPDATE_COMMAND ""
    CMAKE_ARGS
      "-DCMAKE_INSTALL_PREFIX=${wave_install_dir}"
  )
  link_directories("${wave_install_dir}/lib")

  add_executable(rtff_test
    ${src}/rtff/test.cc
  )
  add_dependencies(rtff_test
    external_wave
    external_googletest
  )
  target_link_libraries(rtff_test
    gtest
    gtest_main
    rtff
    wave
    ${mkl_libraries}
  )
  target_include_directories(rtff_test
    PUBLIC
      ${gtest_install_dir}/include
      ${eigen_install_dir}
      ${wave_install_dir}/include
  )
  target_compile_definitions(rtff_test
    PUBLIC
      -DTEST_RESOURCES_PATH="${test_resource_path}"
  )
endif()
