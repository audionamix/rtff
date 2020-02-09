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
  ${src}/rtff/fft/window_type.h
  ${src}/rtff/fft/fft.cc
  ${src}/rtff/fft/fft.h
)
if (${rtff_use_mkl})
  set(rtff_sources ${rtff_sources}
    ${src}/rtff/fft/mkl/mkl_fft.cc
    ${src}/rtff/fft/mkl/mkl_fft.h
    ${src}/rtff/fft/mkl/mkl_fft_context.cc
    ${src}/rtff/fft/mkl/mkl_fft_context.h
  )
else()
  set(rtff_sources ${rtff_sources}
    ${src}/rtff/fft/fftw/fftw_fft.cc
    ${src}/rtff/fft/fftw/fftw_fft.h
  )
endif()

add_library(rtff ${rtff_sources})
target_link_libraries(rtff
  eigen
)
target_include_directories(rtff PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)

OrganizeSources(SOURCES ${rtff_sources})

# Add fftw if we don't want to use the mkl
if (${rtff_use_mkl})
  target_include_directories(rtff
    PUBLIC
      ${mkl_include_dir}
  )
  target_compile_definitions(rtff
    PUBLIC
      -DRTFF_USE_MKL
  )
else()
  target_include_directories(rtff
    PUBLIC
      ${fftw_include_dir}
  )
  if (${rtff_fftw_use_wisdom})
    message(STATUS "Using fftw wisdom files")
    target_compile_definitions(rtff
      PUBLIC
        -DRTFF_FFTW_USE_WISDOM=ON
    )
  endif()
endif()

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
install(FILES
  ${src}/rtff/fft/window_type.h
  DESTINATION include/rtff/fft
)
# - external libraries
set(external_libraries "")
set(external_includes "")
if (${rtff_use_mkl})
  set(external_libraries ${mkl_libraries})
  set(mkl_dependencies "")
  foreach (lib ${external_libraries})
    if (EXISTS ${lib})
      set(mkl_dependencies ${mkl_dependencies} "${lib}")
    endif()
  endforeach()

  install(FILES
    ${mkl_dependencies}
    DESTINATION lib
  )
else()
  set(external_libraries ${fftw_libraries})
  if (UNIX AND NOT APPLE)
      set(external_libraries ${external_libraries} pthread)
  endif()
  set(external_includes ${fftw_include_dir})
  install(FILES
    ${fftw_install_dir}/lib/libfftw3f.a
    DESTINATION lib
  )
endif()

if (${rtff_enable_tests})
  add_executable(rtff_test
    ${src}/rtff/test.cc
    ${src}/rtff/buffer/buffer_test.cc
  )

  if (${rtff_enable_multithread})
    set(external_libraries ${external_libraries} tbb)
  endif()

  target_link_libraries(rtff_test
    gtest
    gtest_main
    rtff
    wave
    eigen
    ${external_libraries}
  )
  target_include_directories(rtff_test
    PUBLIC
      ${external_includes}
  )
  target_compile_definitions(rtff_test
    PUBLIC
      -DTEST_RESOURCES_PATH="${test_resource_path}"
  )
endif()
