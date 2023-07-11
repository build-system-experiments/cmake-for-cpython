include(CheckFunctionExists)

macro(py_check_rt_func FUNC)
    string(TOUPPER "HAVE_${FUNC}" _DEF)
    set(CMAKE_REQUIRED_LIBRARIES rt)
    set(CMAKE_EXTRA_INCLUDE_FILES time.h)
    unset(CMAKE_REQUIRED_INCLUDES)
    check_function_exists(${FUNC} ${_DEF})
    unset(_DEF)
endmacro()
