set_directory_properties(PROPERTIES COMPILE_DEFINITIONS -DPy_BUILD_CORE)

add_executable(_bootstrap_python _bootstrap_python.c)
target_link_libraries(_bootstrap_python
  PRIVATE
    getpath
    libpython_omit_frozen
)

add_executable(_testembed _testembed.c)
target_link_libraries(_testembed
  PRIVATE
    libs
    libpython 
)

add_executable(python python.c)
target_link_libraries(python
  PRIVATE
    libs
    libpython
    # FIXME: shared mods
)

add_executable(_freeze_module _freeze_module.c)
target_link_libraries(_freeze_module
  PRIVATE
    libs
    getpath_noop
    libpython_omit_frozen
)
