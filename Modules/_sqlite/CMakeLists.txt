# FindSQLite3 was introduced with 3.14, and
# version ranges were introduced with 3.19.
cmake_minimum_required(VERSION 3.19)

find_package(SQLite3 REQUIRED "3.15.2<")
message(STATUS "Found SQLite3 ${SQLite3_VERSION}")
message(STATUS "- SQLite3_INCLUDE_DIR: ${SQLite3_INCLUDE_DIRS}")
message(STATUS "- SQLite3_LIBRARIES: ${SQLite3_LIBRARIES}")

option(
    ENABLE_LOADABLE_SQLITE_EXTENSIONS
    "Support loadable extensions in the sqlite3 module, see Doc/library/sqlite3.rst"
    OFF
)

include(cmake/PyCheckSQLiteFunc.cmake)
py_check_sqlite_func(sqlite3_bind_double)
py_check_sqlite_func(sqlite3_column_decltype)
py_check_sqlite_func(sqlite3_column_double)
py_check_sqlite_func(sqlite3_complete)
py_check_sqlite_func(sqlite3_load_extension)
py_check_sqlite_func(sqlite3_progress_handler)
py_check_sqlite_func(sqlite3_result_double)
py_check_sqlite_func(sqlite3_serialize)
py_check_sqlite_func(sqlite3_set_authorizer)
py_check_sqlite_func(sqlite3_trace_v2)
py_check_sqlite_func(sqlite3_value_double)

if(ENABLE_LOADABLE_SQLITE_EXTENSIONS)
    if(NOT PY_SQLITE_ENABLE_LOAD_EXTENSION)
        message(WARNING "Your SQLite library does not support loadable extensions")
    else()
        set(PY_SQLITE_ENABLE_LOAD_EXTENSION 1)
    endif()
endif()

add_library(_sqlite3 OBJECT
    blob.c
    connection.c
    cursor.c
    microprotocols.c
    module.c
    prepare_protocol.c
    row.c
    statement.c
    util.c
)
target_link_libraries(_sqlite3 SQLite::SQLite3)
target_include_directories(_sqlite3 PRIVATE clinic)
