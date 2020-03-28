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
  set(fft_backend RTFF_USE_MKL)
elseif (${rtff_use_fftw})
  set(rtff_sources ${rtff_sources}
    ${src}/rtff/fft/fftw/fftw_fft.cc
    ${src}/rtff/fft/fftw/fftw_fft.h
  )
  set(fft_backend RTFF_USE_FFTW)
else ()
  set(rtff_sources ${rtff_sources}
    ${src}/rtff/fft/eigen/eigen_fft.cc
    ${src}/rtff/fft/eigen/eigen_fft.h
  )
  set(fft_backend RTFF_USE_EIGEN)
endif()

add_library(rtff ${rtff_sources})

target_include_directories(rtff PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)

OrganizeSources(SOURCES ${rtff_sources})

target_link_libraries(rtff
  eigen
  ${external_libraries}
)
<<<<<<< HEAD

if (${rtff_use_mkl})
  target_compile_definitions(rtff
    PUBLIC
      -DRTFF_USE_MKL
  )
elseif (${rtff_fftw_use_wisdom})
  message(STATUS "Using fftw wisdom files")
  target_compile_definitions(rtff
    PUBLIC
      -DRTFF_FFTW_USE_WISDOM=ON
  )
=======
# deal with fftw wisdom
set(compile_definitions -D${fft_backend})
if (${rtff_fftw_use_wisdom})
  message(STATUS "Using fftw wisdom files")
  set(compile_definitions ${compile_definitions} -DRTFF_FFTW_USE_WISDOM=ON)
>>>>>>> ff9382e6145fcf239163015b144f1229be2942b7
endif()
target_compile_definitions(rtff PUBLIC ${compile_definitions})

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

if (${rtff_enable_tests})
  add_executable(rtff_test
    ${src}/rtff/test.cc
    ${src}/rtff/buffer/buffer_test.cc
  )

  target_link_libraries(rtff_test
    gtest
    gtest_main
    rtff
    wave
    eigen
    ${external_libraries}
  )
  target_compile_definitions(rtff_test
    PUBLIC
      -DTEST_RESOURCES_PATH="${test_resource_path}"
  )
endif()
