set(rtff_sources
  ${src}/rtff/filter.cc
  ${src}/rtff/filter.h
  ${src}/rtff/abstract_filter.cc
  ${src}/rtff/abstract_filter.h

  ${src}/rtff/filter_impl.cc
  ${src}/rtff/filter_impl.h

  ${src}/rtff/buffer/ring_buffer.cc
  ${src}/rtff/buffer/ring_buffer.h
  ${src}/rtff/buffer/overlap_ring_buffer.cc
  ${src}/rtff/buffer/overlap_ring_buffer.h
  ${src}/rtff/buffer/audio_buffer.cc
  ${src}/rtff/buffer/audio_buffer.h
  ${src}/rtff/buffer/buffer.h

  ${src}/rtff/fft/window.cc
  ${src}/rtff/fft/window.h
  ${src}/rtff/fft/fft.cc
  ${src}/rtff/fft/fft.h
  ${src}/rtff/fft/mkl/mkl_fft.cc
  ${src}/rtff/fft/mkl/mkl_fft.h
  ${src}/rtff/fft/mkl/mkl_fft_context.cc
  ${src}/rtff/fft/mkl/mkl_fft_context.h
)
add_library(rtff ${rtff_sources})

OrganizeSources(SOURCES ${rtff_sources})

target_include_directories(rtff
  PUBLIC
    ${eigen_install_dir}
    ${mkl_include_dir}
)
add_dependencies(rtff
  external_eigen
)

# install rules
# - built lib
install(TARGETS rtff
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
# - headers
install(FILES
  ${src}/rtff/filter.h
  ${src}/rtff/abstract_filter.h
  DESTINATION include/rtff
)
install(FILES
  ${src}/rtff/buffer/audio_buffer.h
  DESTINATION include/rtff/buffer
)
# - external libraries
set(mkl_dependencies "")
foreach (lib ${mkl_libraries})
  if (EXISTS ${lib})
    set(mkl_dependencies ${mkl_dependencies} "${lib}")
  endif()
endforeach()
install(FILES
  ${mkl_dependencies}
  DESTINATION lib
)

if (${rtff_enable_tests})
  # libwave
  set(wave_install_dir ${CMAKE_BINARY_DIR}/wave-install)
  ExternalProject_Add(external_wave
    GIT_REPOSITORY "https://github.com/audionamix/wave"
    GIT_TAG "e73572dfdc15fba8ac8e544a7f2410adb2588d10"
    # don't update so that we don't rebuild every time
    UPDATE_COMMAND ""
    CMAKE_ARGS
      "-DCMAKE_INSTALL_PREFIX=${wave_install_dir}"
      "-Dwave_enable_tests=OFF"
  )
  link_directories("${wave_install_dir}/lib")

  add_executable(rtff_test
    ${src}/rtff/test.cc
    ${src}/rtff/buffer/buffer_test.cc
  )
  add_dependencies(rtff_test
    external_wave
    external_googletest
  )
  if (${rtff_enable_multithread})
    add_dependencies(rtff_test external_tbb)
  endif()
  if (UNIX AND NOT APPLE)
    target_link_libraries(rtff_test
      gtest
      gtest_main
      rtff
      debug waved
      optimized wave
      ${mkl_libraries}
    )
  else ()
    target_link_libraries(rtff_test
      optimized gtest
      optimized gtest_main
      debug gtestd
      debug gtest_maind
      rtff
      debug waved
      optimized wave
      ${mkl_libraries}
    )
  endif()
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
