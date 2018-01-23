# The Mkl has to be installed locally
# TODO switch static/dynamic
# TODO switch x86 / x64
function(FindMkl)
  # parse args
  set(option_args "")
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

    elseif(WIN32)

    endif()
  endif()

  if (NOT EXISTS ${FindMkl_PATH})
    message(FATAL_ERROR "Can't find Intel MKL")
  endif()

  # link options provided by
  # https://software.intel.com/en-us/articles/intel-mkl-link-line-advisor
  set(MKLROOT ${FindMkl_PATH})
  if (UNIX AND APPLE)
    set(mkl_libraries
      "${MKLROOT}/lib/libmkl_intel.a"
      "${MKLROOT}/lib/libmkl_core.a"
      "${MKLROOT}/lib/libmkl_intel_thread.a"
      "${FindMkl_INTEL_PATH}/lib/libiomp5.a"
      "-lpthread"
      "-lm"
      "-ldl"
      "-L${FindMkl_INTEL_PATH}/lib"
    )
    set(mkl_compiler_options
      "-m32"
    )
    set(mkl_include_dir "${MKLROOT}/include")
  elseif(UNIX AND NOT APPLE)
    set(mkl_libraries
      "-Wl,--start-group"
      "${MKLROOT}/lib/ia32/libmkl_intel.a"
      "${MKLROOT}/lib/ia32/libmkl_core.a"
      "${MKLROOT}/lib/ia32/libmkl_intel_thread.a"
      "-Wl,--end-group"
      "-liomp5"
      "-lpthread"
      "-lm"
      "-ldl"
    )
    set(mkl_compiler_options
      "-m32"
    )
    set(mkl_include_dir "${MKLROOT}/include")
  elseif(WIN32)
    set(mkl_libraries
       "mkl_intel_c.lib"
       "mkl_core.lib"
       "mkl_intel_thread.lib"
       "libiomp5md.lib"
    )
    set(mkl_compiler_options "")
    set(mkl_include_dir "${MKLROOT}/include")
  else()
    message(FATAL_ERROR "Unkown platform")
  endif()

  set(mkl_libraries ${mkl_libraries} PARENT_SCOPE)
  set(mkl_include_dir ${mkl_include_dir} PARENT_SCOPE)
  set(mkl_compiler_options ${mkl_compiler_options} PARENT_SCOPE)
endfunction(FindMkl)
