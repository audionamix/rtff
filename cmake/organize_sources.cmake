function(OrganizeSources)
  # parse args
  set(option_args "")
  set(one_val_args "")
  set(multiple_val_args SOURCES)
  cmake_parse_arguments(OrganizeSources "${option_args}" "${one_val_args}" "${multiple_val_args}" ${ARGN})

  get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} PATH)

  foreach(source_file ${OrganizeSources_SOURCES})
    # find if source or header
    get_filename_component(source_ext ${source_file} EXT)
    get_filename_component(absolute_path ${source_file} ABSOLUTE)
    string(REPLACE ${PARENT_DIR} " " relative_path ${absolute_path})
    string(STRIP ${relative_path} relative_path)
    string(SUBSTRING ${relative_path} 1 -1 relative_path)
    get_filename_component(relative_folder ${relative_path} DIRECTORY)
    string(REPLACE "/" "\\" group_name ${relative_folder})
    # add the file to the right group
    source_group(${group_name} FILES ${source_file})
  endforeach()
endfunction(OrganizeSources)
