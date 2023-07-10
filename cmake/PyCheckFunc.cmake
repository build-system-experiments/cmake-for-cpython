include(CheckFunctionExists)

macro(py_check_func FUNC)
    cmake_parse_arguments(PY_CHECK_FUNC
        ""                  # options
        "RESULT_VAR"   # one-value args
        "HEADERS"           # multi-value args
        ${ARGN}
    )
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_INCLUDES)
    set(CMAKE_EXTRA_INCLUDE_FILES "${PY_CHECK_FUNC_HEADERS}")
    if(${PY_CHECK_FUNC_RESULT_VAR})
        set(_DEF ${PY_CHECK_FUNC_RESULT_VAR})
    else()
        string(TOUPPER "HAVE_${FUNC}" _DEF)
    endif()
    check_function_exists(${FUNC} ${_DEF})
    unset(_DEF)
endmacro()
