include(CheckFunctionExists)

function(py_check_sqlite_func FUNC)
    string(TOUPPER "PY_SQLITE_HAVE_${FUNC}" RETVAL)
    set(CMAKE_REQUIRED_LIBRARIES sqlite3)
    set(CMAKE_REQUIRED_INCLUDES "${SQLite3_INCLUDE_DIRS}")
    check_symbol_exists(${FUNC} sqlite3.h _RET)
    set(${RETVAL} ${_RET} PARENT_SCOPE)
endfunction()
