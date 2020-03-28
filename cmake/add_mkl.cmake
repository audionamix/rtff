# The Mkl has to be installed locally
# TODO switch static/dynamic
# TODO switch x86 / x64
function(FindMkl)
  # parse args
  set(option_args MULTITHREADING)
  set(multiple_val_args "")
  set(one_val_args PATH INTEL_PATH)
  cmake_parse_arguments(FindMkl
    "${option_args}" "${one_val_args}" "${multiple_val_args}" "${ARGN}"
  )

  # if not path provided, use mklroot default values
  if (NOT FindMkl_PATH)
    if (UNIX AND APPLE)
      if (NOT FindMkl_INTEL_PATH)
        set(FindMkl_INTEL_PATH /opt/intel)
      endif()
      set(FindMkl_PATH ${FindMkl_INTEL_PATH}/mkl)
    elseif (UNIX AND NOT APPLE)
      if (NOT FindMkl_INTEL_PATH)
        set(FindMkl_INTEL_PATH /opt/intel)
      endif()
      set(FindMkl_PATH ${FindMkl_INTEL_PATH}/mkl)

    elseif(WIN32)
      set(program_files_env_var "PROGRAMFILES(X86)")
      string(REPLACE "\\" "/" program_files $ENV{${program_files_env_var}})
      if (NOT FindMkl_INTEL_PATH)
        set(FindMkl_INTEL_PATH ${program_files}/IntelSWTools/compilers_and_libraries/windows)
      endif()
      set(FindMkl_PATH ${FindMkl_INTEL_PATH}/mkl)
    endif()
  endif()

  if (NOT EXISTS "${FindMkl_PATH}")
    message(FATAL_ERROR "Can't find Intel MKL. ${FindMkl_PATH} doesn't exist")
  endif()

  # link options provided by
  # https://software.intel.com/en-us/articles/intel-mkl-link-line-advisor
  set(MKLROOT ${FindMkl_PATH})
  if (UNIX AND APPLE)
    set(mkl_libraries
      "${MKLROOT}/lib/libmkl_intel_lp64.a"
      "${MKLROOT}/lib/libmkl_core.a"
      "-lpthread"
      "-lm"
      "-ldl"
    )
    set(mkl_compiler_options
      "-m32"
    )
    set(mkl_include_dir "${MKLROOT}/include")
    if (${FindMkl_MULTIHREADING})
      set(mkl_libraries ${mkl_libraries} "${MKLROOT}/lib/libmkl_tbb_thread.a" "-ltbb")
    else()
      set(mkl_libraries ${mkl_libraries} "${MKLROOT}/lib/libmkl_sequential.a")
    endif()
  elseif(UNIX AND NOT APPLE)
    if (${FindMkl_MULTIHREADING})
      set(mkl_libraries
        "-Wl,--start-group"
        "${MKLROOT}/lib/intel64/libmkl_intel_lp64.a"
        "${MKLROOT}/lib/intel64/libmkl_core.a"
        "${MKLROOT}/lib/intel64/libmkl_tbb_thread.a"
        "-Wl,--end-group"
        "-ltbb"
        "-lpthread"
        "-lm"
        "-ldl"
      )
    else()
      set(mkl_libraries
        "-Wl,--start-group"
        "${MKLROOT}/lib/intel64/libmkl_intel_lp64.a"
        "${MKLROOT}/lib/intel64/libmkl_core.a"
        "${MKLROOT}/lib/intel64/libmkl_sequential.a"
        "-Wl,--end-group"
        "-lpthread"
        "-lm"
        "-ldl"
      )
    endif()
    set(mkl_compiler_options
      "-m32"
    )
    set(mkl_include_dir "${MKLROOT}/include")
  elseif(WIN32)
    if (CMAKE_SIZEOF_VOID_P MATCHES "8")
      # 64bits
      set(mkl_libraries
         "${MKLROOT}/lib/intel64/mkl_intel_lp64.lib"
         "${MKLROOT}/lib/intel64/mkl_core.lib"
      )
      if (${FindMkl_MULTIHREADING})
        set(mkl_libraries ${mkl_libraries} "${MKLROOT}/lib/intel64/mkl_tbb_thread.lib")
      else()
        set(mkl_libraries ${mkl_libraries} "${MKLROOT}/lib/intel64/mkl_sequential.lib")
      endif()
    else ()
      # 32bits
      set(mkl_libraries
         "${MKLROOT}/lib/ia32/mkl_intel_c.lib"
         "${MKLROOT}/lib/ia32/mkl_core.lib"
      )
      if (${FindMkl_MULTIHREADING})
        set(mkl_libraries ${mkl_libraries} "${MKLROOT}/lib/ia32/mkl_tbb_thread.lib")
      else()
        set(mkl_libraries ${mkl_libraries} "${MKLROOT}/lib/ia32/mkl_sequential.lib")
      endif()
    endif ()
    set(mkl_compiler_options "")
    set(mkl_include_dir "${MKLROOT}/include")
  else()
    message(FATAL_ERROR "Unkown platform")
  endif()

  add_library(mkl INTERFACE)
  target_link_options(mkl INTERFACE ${mkl_libraries})
  target_include_directories(mkl INTERFACE ${mkl_include_dir})
  # If included, leads to errors
  # target_compile_options(mkl INTERFACE ${mkl_compiler_options})

  set(mkl_libraries ${mkl_libraries} PARENT_SCOPE)
  set(mkl_include_dir ${mkl_include_dir} PARENT_SCOPE)
  set(mkl_compiler_options ${mkl_compiler_options} PARENT_SCOPE)
endfunction(FindMkl)

FindMkl()
install(FILES ${mkl_libraries} DESTINATION lib)
