include(CheckFunctionExists)
#
# SOCKET_HEADERS must be set by parent scope.
#
macro(py_check_socket_func FUNC)
    string(TOUPPER "HAVE_${FUNC}" _DEF)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_INCLUDES)
    set(CMAKE_EXTRA_INCLUDE_FILES "${SOCKET_HEADERS}")
    check_function_exists(${FUNC} ${_DEF})
    unset(_DEF)
endmacro()
