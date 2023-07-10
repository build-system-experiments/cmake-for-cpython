# OBJECT libraries were introduced in 3.12
# LZMA support introduced in 3.14
cmake_minimum_required(VERSION 3.14)

# FIXME: not every stdlib ext module should get this flag
set_directory_properties(PROPERTIES COMPILE_DEFINITIONS -DPy_BUILD_CORE)

include(CheckLibraryExists)
include(CheckIncludeFile)

add_library(getpath OBJECT getpath.c)
add_library(getpath_noop OBJECT getpath_noop.c)
add_library(getbuildinfo OBJECT getbuildinfo.c)
add_library(_modules OBJECT)

function(py_stdlib_mod_simple NAME FILE)
    option(ENABLE_${NAME} "Enable the ${NAME} extension module" ON)
    message(STATUS "adding simple module ${NAME}")
    set(LIBNAME ${NAME})
    add_library(${LIBNAME} OBJECT ${FILE})
    target_sources(_modules PRIVATE $<TARGET_OBJECTS:${LIBNAME}>)
endfunction()

py_stdlib_mod_simple(_abc _abc.c)
py_stdlib_mod_simple(_asyncio _asynciomodule.c)
py_stdlib_mod_simple(_bisectmodule _bisectmodule.c)

option(ENABLE__BZ2 "Enable the _bz2 extension module" ON)
if(ENABLE__BZ2)
    check_include_file(bzlib.h HAVE_BZLIB_H)
    find_package(BZip2)
    if(BZIP2_FOUND)
        py_stdlib_mod_simple(_bz2 _bz2module.c)
        target_include_directories(_bz2 PRIVATE "${BZIP2_INCLUDE_DIRS}")
        target_link_libraries(_bz2 PRIVATE "${BZIP2_LIBRARIES}")
    endif()
    message(STATUS "- BZIP2_LIBRARIES: ${BZIP2_LIBRARIES}")
    message(STATUS "- BZIP2_INCLUDE_DIRS: ${BZIP2_INCLUDE_DIRS}")
endif()

py_stdlib_mod_simple(_codecs _codecsmodule.c)
py_stdlib_mod_simple(_collections _collectionsmodule.c)
py_stdlib_mod_simple(_contextvars _contextvarsmodule.c)
py_stdlib_mod_simple(_csv _csv.c)

option(ENABLE__CURSES "Enable the _curses extension module" ON)
if(ENABLE__CURSES)
    find_package(Curses)
    if(CURSES_FOUND)
        py_stdlib_mod_simple(_curses _cursesmodule.c)
        #target_sources(_curses PRIVATE _curses_panel.c)  # FIXME
        target_include_directories(_curses PRIVATE "${CURSES_INCLUDE_DIRS}")
        target_compile_options(_curses PRIVATE ${CURSES_CFLAGS})
        target_link_libraries(_curses ${CURSES_LIBRARIES})
        message(STATUS "- CURSES_INCLUDE_DIRS: ${CURSES_INCLUDE_DIRS}")
        message(STATUS "- CURSES_LIBRARIES: ${CURSES_LIBRARIES}")
        message(STATUS "- CURSES_CFLAGS: ${CURSES_CFLAGS}")
    endif()
    if(CURSES_HAVE_CURSES_H)
        set(HAVE_CURSES_H TRUE)
    endif()
    if(CURSES_HAVE_NCURSES_H)
        set(HAVE_NCURSES_H TRUE)
    endif()
endif()

py_stdlib_mod_simple(_datetime _datetimemodule.c)

check_include_file(gdbm/ndbm.h HAVE_GDBM_NDBM_H)
check_include_file(gdbm-ndbm.h HAVE_GDBM_DASH_NDBM_H)

option(ENABLE__DBM "Enable the _dbm extension module" ON)
if(ENABLE__DBM)
    check_include_file(ndbm.h HAVE_NDBM_H)
    if(HAVE_NDBM_H)
        function(check_ndbm RESULT ITEM)
            message(STATUS "check_ndbm: RESULT=${RESULT} ITEM=${ITEM}")
            set(${RESULT} TRUE PARENT_SCOPE)
        endfunction()
        find_library(NDBM
          NAMES
            ndbm
            gdbm_compat
          VALIDATOR check_ndbm
        )
        message(STATUS "NDBM=${NDBM}")
        py_stdlib_mod_simple(_dbm _dbmmodule.c)
    endif()
endif()

py_stdlib_mod_simple(_elementtree _elementtree.c)
py_stdlib_mod_simple(_functools _functoolsmodule.c)

option(ENABLE__GDBM "Enable the _gdbm extension module" ON)
if(ENABLE__GDBM)
    check_include_file(gdbm.h HAVE_GDBM_H)
    if(HAVE_GDBM_H)
        function(check_gdbm RESULT ITEM)
            message(STATUS "check_gdbm: RESULT=${RESULT} ITEM=${ITEM}")
            set(${RESULT} TRUE PARENT_SCOPE)
        endfunction()
        find_library(GDBM
          NAMES gdbm
          VALIDATOR check_gdbm
        )
        message(STATUS "GDBM=${GDBM}")
        py_stdlib_mod_simple(_gdbm _gdbmmodule.c)
    endif()
endif()

py_stdlib_mod_simple(_heapq _heapqmodule.c)
py_stdlib_mod_simple(_json _json.c)
py_stdlib_mod_simple(_locale _localemodule.c)
py_stdlib_mod_simple(_lsprof _lsprof.c)

option(ENABLE__LZMA "Enable the _lzma extension module" ON)
if(ENABLE__LZMA)
    find_package(LibLZMA)
    if(LIBLZMA_FOUND)
        message(STATUS "- LIBLZMA_VERSION=${LIBLZMA_VERSION}")  # Version 3.26 only!
        message(STATUS "- LIBLZMA_INCLUDE_DIRS=${LIBLZMA_INCLUDE_DIRS}")
        message(STATUS "- LIBLZMA_LIBRARIES=${LIBLZMA_LIBRARIES}")
        message(STATUS "- LIBLZMA_HAS_AUTO_DECODER=${LIBLZMA_HAS_AUTO_DECODER}")
        message(STATUS "- LIBLZMA_HAS_EASY_ENCODER=${LIBLZMA_HAS_EASY_ENCODER}")
        message(STATUS "- LIBLZMA_HAS_LZMA_PRESET=${LIBLZMA_HAS_LZMA_PRESET}")
        py_stdlib_mod_simple(_lzma _lzmamodule.c)
        add_dependencies(_lzma LibLZMA::LibLZMA)
    endif()
endif()

py_stdlib_mod_simple(_opcode _opcode.c)
py_stdlib_mod_simple(_operator _operator.c)
py_stdlib_mod_simple(_pickle _pickle.c)
py_stdlib_mod_simple(_posixsubprocess _posixsubprocess.c)

py_stdlib_mod_simple(_queue _queuemodule.c)
py_stdlib_mod_simple(_random _randommodule.c)
py_stdlib_mod_simple(_scproxy _scproxy.c)

find_package(OpenSSL)
if(OPENSSL_FOUND)
    message(STATUS "- OPENSSL_VERSION=${OPENSSL_VERSION}")
    message(STATUS "- OPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR}")
    message(STATUS "- OPENSSL_CRYPTO_LIBRARY=${OPENSSL_CRYPTO_LIBRARY}")
    message(STATUS "- OPENSSL_CRYPTO_LIBRARIES=${OPENSSL_CRYPTO_LIBRARIES}")
    message(STATUS "- OPENSSL_SSL_LIBRARY=${OPENSSL_SSL_LIBRARY}")
    message(STATUS "- OPENSSL_SSL_LIBRARIES=${OPENSSL_SSL_LIBRARIES}")
    message(STATUS "- OPENSSL_LIBRARIES=${OPENSSL_LIBRARIES}")
    py_stdlib_mod_simple(_hashopenssl _hashopenssl.c)
    target_include_directories(_hashopenssl PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(_hashopenssl PRIVATE ${OPENSSL_LIBRARIES})

    py_stdlib_mod_simple(_ssl _ssl.c)
    target_include_directories(_ssl PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(_ssl PRIVATE ${OPENSSL_LIBRARIES})
endif()

py_stdlib_mod_simple(_stat _stat.c)
py_stdlib_mod_simple(_statistic _statisticsmodule.c)
py_stdlib_mod_simple(_struct _struct.c)
py_stdlib_mod_simple(_thread _threadmodule.c)

option(ENABLE__TKINTER "Enable the _tkinter extension module" ON)
if(ENABLE__TKINTER)
    find_package(TCL)
    find_package(X11)
    if(TCLTK_FOUND)
        message(STATUS "- TCLTK_FOUND=${TCLTK_FOUND}")
        message(STATUS "- TCL_LIBRARY=${TCL_LIBRARY}")
        message(STATUS "- TCL_INCLUDE_PATH=${TCL_INCLUDE_PATH}")
        message(STATUS "- TK_LIBRARY=${TK_LIBRARY}")
        message(STATUS "- TK_INCLUDE_PATH=${TK_INCLUDE_PATH}")
        message(STATUS "- X11_INCLUDE_DIR=${X11_INCLUDE_DIR}")
        message(STATUS "- X11_LIBRARIES=${X11_LIBRARIES}")
        py_stdlib_mod_simple(_tkinter _tkinter.c)
        target_include_directories(_tkinter
          PRIVATE
            "${TCL_INCLUDE_PATHS}"
            "${TK_INCLUDE_PATHS}"
            "${X11_INCLUDE_DIR}"
        )
        target_link_libraries(_tkinter
          PRIVATE
            "${TCL_LIBRARY}"
            "${TK_LIBRARY}"
            "${X11_LIBRARIES}"
        )
        target_compile_options(_tkinter
          PRIVATE
            -Wno-strict-prototypes
            -DWITH_APPINIT=1
        )
    else()
        message(WARNING "Unable to find Tcl/Tk")
    endif()
endif()

py_stdlib_mod_simple(_tracemalloc _tracemalloc.c)
py_stdlib_mod_simple(_typing _typingmodule.c)
py_stdlib_mod_simple(_uuid _uuidmodule.c)
py_stdlib_mod_simple(_weakref _weakref.c)

if(WIN32)
    py_stdlib_mod_simple(_winapi _winapi.c)
    py_stdlib_mod_simple(overlapped overlapped.c)
endif()

py_stdlib_mod_simple(_xxinterpchannels _xxinterpchannelsmodule.c)
py_stdlib_mod_simple(_xxsubinterpreters _xxsubinterpretersmodule.c)
py_stdlib_mod_simple(_zoneinfo _zoneinfo.c)
py_stdlib_mod_simple(array arraymodule.c)
py_stdlib_mod_simple(atexit atexitmodule.c)
py_stdlib_mod_simple(binascii binascii.c)
py_stdlib_mod_simple(cmath cmathmodule.c)
py_stdlib_mod_simple(errno errnomodule.c)
py_stdlib_mod_simple(faulthandler faulthandler.c)
py_stdlib_mod_simple(fcntl fcntlmodule.c)
py_stdlib_mod_simple(gc gcmodule.c)
py_stdlib_mod_simple(getaddrinfo getaddrinfo.c)
py_stdlib_mod_simple(getnameinfo getnameinfo.c)
py_stdlib_mod_simple(grp grpmodule.c)
py_stdlib_mod_simple(itertools itertoolsmodule.c)
py_stdlib_mod_simple(math mathmodule.c)
py_stdlib_mod_simple(mmap mmapmodule.c)
py_stdlib_mod_simple(_posix posixmodule.c)
py_stdlib_mod_simple(pwd pwdmodule.c)
py_stdlib_mod_simple(pyexpat pyexpat.c)
py_stdlib_mod_simple(readline readline.c)
py_stdlib_mod_simple(resource resource.c)
py_stdlib_mod_simple(rotatingtree rotatingtree.c)
py_stdlib_mod_simple(select selectmodule.c)
py_stdlib_mod_simple(signal signalmodule.c)
py_stdlib_mod_simple(socket socketmodule.c)
py_stdlib_mod_simple(symtable symtablemodule.c)
py_stdlib_mod_simple(syslog syslogmodule.c)
py_stdlib_mod_simple(termios termios.c)
py_stdlib_mod_simple(time timemodule.c)
py_stdlib_mod_simple(tkappinit tkappinit.c)
py_stdlib_mod_simple(unicodedata unicodedata.c)
py_stdlib_mod_simple(xxlimited xxlimited.c)
py_stdlib_mod_simple(xxlimited_35 xxlimited_35.c)
py_stdlib_mod_simple(xxmodule xxmodule.c)
py_stdlib_mod_simple(xxsubtype xxsubtype.c)
py_stdlib_mod_simple(zlib zlibmodule.c)

py_stdlib_mod_simple(_md5 md5module.c)
py_stdlib_mod_simple(_sha1 sha1module.c)
py_stdlib_mod_simple(_sha2 sha2module.c)

target_link_libraries(_md5 PRIVATE _hacl)
target_link_libraries(_sha1 PRIVATE _hacl)
target_link_libraries(_sha2 PRIVATE _hacl)

py_stdlib_mod_simple(sha3 sha3module.c)

option(ENABLE__SQLITE3 "Enable the _sqlite3 extension module" ON)
if(ENABLE__SQLITE3)
    add_subdirectory(_sqlite)
    target_sources(_modules
      PRIVATE
        $<TARGET_OBJECTS:_sqlite3>
    )
endif()

py_stdlib_mod_simple(_testbuffer _testbuffer.c)

set_source_files_properties(_testcapimodule.c PROPERTIES COMPILE_DEFINITIONS "")
py_stdlib_mod_simple(_testcapi _testcapimodule.c)

py_stdlib_mod_simple(_testclinic _testclinic.c)
py_stdlib_mod_simple(_testimportmultiple _testimportmultiple.c)
py_stdlib_mod_simple(_testinternalcapi _testinternalcapi.c)
py_stdlib_mod_simple(_testmultiphase _testmultiphase.c)
py_stdlib_mod_simple(_testsinglephase _testsinglephase.c)
