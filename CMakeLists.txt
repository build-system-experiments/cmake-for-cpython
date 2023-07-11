# OBJECT libraries were introduced in 3.12
# find_package version ranges are supported from 3.19
cmake_minimum_required(VERSION 3.19)

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)

include(cmake/PyCoerceInt.cmake)

set(PYTHON_VERSION "3.13.0")
set(VERSION ${PYTHON_VERSION})
project(CPython VERSION ${PYTHON_VERSION} LANGUAGES C ASM)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    # FIXME
    set(ALT_SOABI "\"cpython-${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}-darwin\"")
endif()

#
# Check for various properties of floating point
#
if(CMAKE_C_BYTE_ORDER MATCHES BIG_ENDIAN)  # FIXME
    set(DOUBLE_IS_BIG_ENDIAN_IEEE754 1)
elseif(CMAKE_C_BYTE_ORDER MATCHES LITTLE_ENDIAN)
    set(DOUBLE_IS_LITTLE_ENDIAN_IEEE754 1)
else()
    # Some ARM platforms use a mixed-endian representation for doubles.
    # While Python doesn't currently have full support for these platforms
    # (see e.g., issue 1762561), we can at least make sure that float <-> string
    # conversions work.
    # FLOAT_WORDS_BIGENDIAN doesnt actually detect this case, but if it's not big
    # or little, then it must be this?
    set(DOUBLE_IS_ARM_MIXED_ENDIAN_IEEE754 1)
endif()

#
# Find Python for regen.
#
set(PYTHON_FOR_REGEN "" CACHE STRING "Python for regen")
if (PYTHON_FOR_REGEN)
    message(STATUS "Using PYTHON_FOR_REGEN from option: ${PYTHON_FOR_REGEN}")
else()
    if(DEFINED ENV{PYTHON_FOR_REGEN})
        set(PYTHON_FOR_REGEN ENV{PYTHON_FOR_REGEN})
        message(STATUS "Using PYTHON_FOR_REGEN from env: ${PYTHON_FOR_REGEN}")
    else()
        find_package(Python3 COMPONENTS Interpreter)  # FIXME: version range
        if(Python3_FOUND)
            set(PYTHON_FOR_REGEN "${Python3_EXECUTABLE}")
        else()
            message(WARNING "PYTHON_FOR_REGEN missing")
        endif()
    endif()
endif()
set(UPDATE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Tools/build/update_file.py")

option(ENABLE_IPV6, "Enable IPv6 (with IPv4) support. See Doc/library/socket.rst" ON)
option(WITH_PYMALLOC "Enable specialized mallocs" ON)
option(WITH_FREELISTS "Enable object freelists" ON)
option(WITH_DOC_STRINGS "Enable documentation strings" ON)

py_coerce_int(ENABLE_IPV6)
py_coerce_int(WITH_PYMALLOC)
py_coerce_int(WITH_FREELISTS)
py_coerce_int(WITH_DOC_STRINGS)

if(APPLE)
    option(ENABLE_FRAMEWORK "Create a Python.framework rather than a traditional Unix install" OFF)
    set(PYTHONFRAMEWORK "Python" CACHE STRING "Python framework name")
    set(_PYTHONFRAMEWORK "\"${PYTHONFRAMEWORK}\"")
    set(_DARWIN_C_SOURCE 1)
endif()
if(UNIX)
    set(__BSD_VISIBLE 1)
    set(_NETBSD_SOURCE 1)
endif()

#
# Platform triplet FIXME
#
include(cmake/PyCheckCSourceRuns.cmake)
py_check_c_source_runs(platform-triplet.c UNUSED)

#
# Check for broken sem_getvalue
#
py_check_c_source_fails(
    check-broken-sem-getvalue.c
    HAVE_BROKEN_SEM_GETVALUE)

py_check_c_source_runs(check-builtin-atomic.c HAVE_BUILTIN_ATOMIC)
py_check_c_source_runs(check-stdatomic.c HAVE_STD_ATOMIC)
py_check_c_source_runs(check-chflags.c HAVE_CHFLAGS)

if(UNIX AND ${CMAKE_SYSTEM_NAME} MATCHES Linux)
    py_check_c_source_runs(check-getrandom-syscall.c HAVE_GETRANDOM_SYSCALL)
endif()
py_check_c_source_runs(check-getrandom.c HAVE_GETRANDOM)

#
# Check required and optional headers.
#
set(LINUX_HEADERS
    linux/auxvec.h
    linux/can.h
    linux/can/bcm.h
    linux/can/j1939.h
    linux/can/raw.h
    linux/fs.h
    linux/memfd.h
    linux/netlink.h
    linux/qrtr.h
    linux/random.h
    linux/soundcard.h
    linux/tipc.h
    linux/vm_sockets.h
    linux/wait.h
    netcan/can.h
)
set(COMMON_HEADERS
    alloca.h asm/types.h
    bluetooth.h bluetooth/bluetooth.h
    conio.h
    direct.h dirent.h
    endian.h errno.h
    fcntl.h
    ieeefp.h io.h
    langinfo.h libintl.h libutil.h
    net/ethernet.h net/if.h netpacket/packet.h
    poll.h process.h pty.h

    sys/auxv.h sys/audioio.h sys/bsdtty.h sys/devpoll.h sys/endian.h
    sys/epoll.h sys/event.h sys/file.h sys/ioctl.h sys/kern_control.h
    sys/loadavg.h sys/lock.h sys/mkdev.h sys/modem.h sys/param.h sys/poll.h
    sys/random.h sys/resource.h sys/select.h sys/sendfile.h sys/soundcard.h
    sys/statvfs.h sys/sys_domain.h sys/syscall.h sys/sysmacros.h sys/termio.h
    sys/time.h sys/times.h sys/types.h sys/uio.h sys/un.h sys/utsname.h
    sys/wait.h sys/xattr.h

    sched.h setjmp.h shadow.h signal.h spawn.h stropts.h sysexits.h syslog.h
    termios.h
    util.h utime.h utmp.h uuid.h uuid/uuid.h
    wchar.h
)
list(APPEND HEADERS ${COMMON_HEADERS})
if(UNIX)
    list(APPEND HEADERS ${LINUX_HEADERS})
endif()
foreach(HEADER IN LISTS COMMON_HEADERS)
    string(MAKE_C_IDENTIFIER ${HEADER} _DEF)
    string(TOUPPER "HAVE_${_DEF}" _DEF)
    check_include_file(${HEADER} ${_DEF})
    unset(_DEF)
endforeach()

#
# Check required and optional types.
#
set(EXTRA_TYPES
    fpos_t
    off_t
    pid_t
    uintptr_t
    wchar_t
)
if(HAVE_SYS_TYPES_H)
    set(CMAKE_EXTRA_INCLUDE_FILES sys/types.h)
endif()
foreach(TYPE IN LISTS EXTRA_TYPES)
    string(MAKE_C_IDENTIFIER "${TYPE}" _DEF)
    string(TOUPPER "SIZEOF_${_DEF}" _DEF)
    check_type_size("${TYPE}" ${_DEF})
    unset(_DEF)
endforeach()
if(HAVE_TIME_H)
    list(APPEND CMAKE_EXTRA_INCLUDE_FILES time.h)
endif()
check_type_size(time_t SIZEOF_TIME_T)

set(BUILTIN_TYPES
    "_Bool"
    "double"
    "float"
    "int"
    "long double"
    "long long"
    "long"
    "short"
    "size_t"
)
foreach(TYPE IN LISTS BUILTIN_TYPES)
    string(MAKE_C_IDENTIFIER "${TYPE}" _DEF)
    string(TOUPPER "SIZEOF_${_DEF}" _DEF)
    check_type_size("${TYPE}" ${_DEF} BUILTIN_TYPES_ONLY)
    unset(_DEF)
endforeach()
check_type_size("void *" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)
check_type_size(__uint128_t SIZEOF_GCC_UINT128_T BUILTIN_TYPES_ONLY)
if(SIZEOF_GCC_UINT128_T)
    set(HAVE_GCC_UINT128_T 1)
endif()

set(TYPES
    "long"
    "size_t"
    "ssize_t"
    "max_align_t"
)
include(cmake/AlignOf.cmake)
foreach(TYPE IN LISTS TYPES)
    string(MAKE_C_IDENTIFIER "${TYPE}" _DEF)
    string(TOUPPER ${_DEF} _DEF)
    alignof("${TYPE}" c ${_DEF})
    unset(_DEF)
endforeach()
if(SIZEOF_SSIZE_T)
    set(HAVE_SSIZE_T 1)
endif()

#
# Check required and optional functions and macros.
#
set(SYMBOLS
    accept4 alarm bind_textdomain_codeset chmod chown clock close_range
    confstr copy_file_range ctermid dup dup2 dup3 execv explicit_bzero explicit_memset
    faccessat fchmod fchmodat fchown fchownat fdopendir fdwalk fexecve
    fork fork1 fpathconf fstatat ftime ftruncate futimens futimes futimesat
    gai_strerror getegid getentropy geteuid getgid getgrgid getgrgid_r
    getgrnam_r getgrouplist getgroups gethostname getitimer getloadavg getlogin
    getpeername getpgid getpid getppid getpriority _getpty
    getpwent getpwnam_r getpwuid getpwuid_r getresgid getresuid getrusage getsid getspent
    getspnam getuid getwd if_nameindex initgroups kill killpg lchown linkat
    lockf lstat lutimes madvise mbrtowc memrchr mkdirat mkfifo mkfifoat
    mknod mknodat mktime mmap mremap nice openat pathconf pause pipe
    pipe2 plock poll posix_fadvise posix_fallocate posix_spawn posix_spawnp
    pread preadv preadv2 pthread_condattr_setclock pthread_init pthread_kill
    pwrite pwritev pwritev2 readlink readlinkat readv realpath renameat
    rtpSpawn sched_get_priority_max sched_rr_get_interval sched_setaffinity
    sched_setparam sched_setscheduler sem_clockwait sem_getvalue sem_open
    sem_timedwait sem_unlink sendfile setegid seteuid setgid sethostname
    setitimer setlocale setpgid setpgrp setpriority setregid setresgid
    setresuid setreuid setsid setuid setvbuf shutdown sigaction sigaltstack
    sigfillset siginterrupt sigpending sigrelse sigtimedwait sigwait
    sigwaitinfo snprintf splice strftime strlcpy strsignal symlinkat sync
    sysconf system tcgetpgrp tcsetpgrp tempnam timegm times tmpfile
    tmpnam tmpnam_r truncate ttyname umask uname unlinkat utimensat utimes vfork
    wait wait3 wait4 waitid waitpid wcscoll wcsftime wcsxfrm wmemcmp writev

    fseek64 fseeko fstatvfs ftell64 ftello statvfs
)
foreach(SYMBOL IN LISTS SYMBOLS)
    string(MAKE_C_IDENTIFIER ${SYMBOL} _DEF)
    string(TOUPPER "HAVE_${_DEF}" _DEF)
    check_function_exists(${SYMBOL} ${_DEF})
    unset(_DEF)
endforeach()


check_function_exists(opendir HAVE_OPENDIR)
check_symbol_exists(dirfd "sys/types.h;dirent.h" HAVE_DIRFD)
py_check_c_source_runs(check-dirent-d-type.c HAVE_DIRENT_D_TYPE)

#
# Check for functions and symbols.
#
check_include_file(dlfcn.h HAVE_DLFCN_H)
set(RTLD_SYMBOLS
    RTLD_LAZY
    RTLD_NOW
    RTLD_GLOBAL
    RTLD_LOCAL
    RTLD_NODELETE
    RTLD_NOLOAD
    RTLD_DEEPBIND
    RTLD_MEMBER
)
foreach(SYM IN LISTS RTLD_SYMBOLS)
    string(TOUPPER "HAVE_${SYM}" _DEF)
    check_symbol_exists(${SYM} dlfcn.h ${_DEF})
    unset(_DEF)
endforeach()

include(cmake/PyCheckFunc.cmake)
set(FUNCS
    chroot
    link
    symlink
    fchdir
    fsync
    fdatasync
    getpagesize
)
foreach(FUNC IN LISTS FUNCS)
    py_check_func(${FUNC} HEADERS unistd.h)
endforeach()

check_library_exists(sendfile sendfile
    "sys/types.h;sys/socket.h;sys/uio.h"
    HAVE_SENDFILE
)

check_include_file(grp.h HAVE_GRP_H)
set(HEADERS unistd.h)
if(HAVE_GRP_H)
    list(APPEND HEADERS grp.h)
endif()
string(JOIN " " HEADERS ${HEADERS})
py_check_func(setgroups HEADERS ${HEADERS})
unset(HEADERS)
py_check_func(lchmod)
py_check_func(epoll_create HEADERS sys/epoll.h RESULT_VAR HAVE_EPOLL)
py_check_func(epoll_create1 HEADERS unistd.h)
py_check_func(kqueue HEADERS sys/types.h sys/event.h)
py_check_func(prlimit HEADERS sys/time.h sys/resource.h)

#
# Check for openpty, login_tty and forkpty
#
check_function_exists(openpty HAVE_OPENPTY)
if(HAVE_OPENPTY)
    check_library_exists(util openpty util.h _NEED_UTIL)
    check_library_exists(bsd openpty "" _NEED_BSD)
endif()
check_library_exists(util login_tty util.h HAVE_LOGIN_TTY)
check_function_exists(forkpty HAVE_FORKPTY)
if(HAVE_FORK_PTY)
    check_library_exists(util forkpty util.h _NEED_UTIL)
    check_library_exists(bsd forkpty "" _NEED_UTIL)
endif()

add_library(libs INTERFACE)
if(_NEED_UTIL)
    target_link_libraries(libs INTERFACE util)
endif()
if(_NEED_BSD)
    target_link_libraries(libs INTERFACE bsd)
endif()

#
# Check for get/set group.
#
check_function_exists(getpgrp HAVE_GETPGRP)
if(HAVE_GETPGRP)
    py_check_c_source_runs(check-getpgrp.c GETPGRP_HAVE_ARG)
endif()
check_function_exists(setpgrp HAVE_SETPGRP)
if(HAVE_SETPGRP)
    py_check_c_source_runs(check-setpgrp.c SETPGRP_HAVE_ARG)
endif()

check_include_file(sys/mman.h HAVE_SYS_MMAN_H)
check_include_file(sys/memfd.h HAVE_SYS_MEMFD_H)
if(HAVE_SYS_MMAN_H)
    list(APPEND HEADERS sys/mman.h)
endif()
if(HAVE_SYS_MEMFD_H)
    list(APPEND HEADERS sys/memfd.h)
endif()
py_check_func(memfd_create HEADERS "${HEADERS}")

check_include_file(sys/eventfd.h HAVE_SYS_EVENTFD_H)
py_check_func(eventfd HEADERS sys/eventfd.h)
py_check_func(ctermid_r HEADERS stdio.h)

check_function_exists(setns HAVE_SETNS)
check_function_exists(unshare HAVE_UNSHARE)

#
# Check for rt functions.
#
include(cmake/PyCheckRtFunc.cmake)
py_check_rt_func(clock_getres)
py_check_rt_func(clock_gettime)
py_check_rt_func(clock_nanosleep)
py_check_rt_func(clock_settime)
py_check_rt_func(nanosleep)

#
# Check for math functions.
#
set(MATH_FUNCS
    acosh
    asinh
    atanh
    erf
    erfc
    expm1
    log1p
    log2
)
foreach(FUNC IN LISTS MATH_FUNCS)
    string(TOUPPER "HAVE_${FUNC}" _DEF)
    check_library_exists(m ${FUNC} math.h ${_DEF})
    unset(_DEF)
endforeach()

#
# Check for netdb functions.
#
check_include_file(netdb.h HAVE_NETDB_H)
include(cmake/PyCheckNetdbFunc.cmake)
py_check_netdb_func(hstrerror)
py_check_netdb_func(getservbyname)
py_check_netdb_func(getservbyport)
py_check_netdb_func(gethostbyname)
py_check_netdb_func(gethostbyaddr)
py_check_netdb_func(getprotobyname)

set(CMAKE_EXTRA_INCLUDE_FILES "netdb.h")
check_type_size("struct addrinfo" SIZEOF_ADDRINFO)
if(SIZEOF_ADDRINFO)
    set(HAVE_ADDRINFO 1)
endif()

#
# Check for socket functions.
#
set(SOCKET_HEADERS
    sys/types.h
    sys/socket.h
    netinet/in.h
    arpa/inet.h
)
foreach(HEADER IN LISTS SOCKET_HEADERS)
    string(MAKE_C_IDENTIFIER ${HEADER} _DEF)
    string(TOUPPER "HAVE_${_DEF}" _DEF)
    check_include_file(${HEADER} ${_DEF})
    unset(_DEF)
endforeach()
include(cmake/PyCheckSocketFunc.cmake)
py_check_socket_func(accept)
py_check_socket_func(bind)
py_check_socket_func(connect)
py_check_socket_func(getpeername)
py_check_socket_func(getsockname)
py_check_socket_func(inet_aton)
py_check_socket_func(inet_ntoa)
py_check_socket_func(listen)
py_check_socket_func(recvfrom)
py_check_socket_func(sendto)
py_check_socket_func(setsockopt)
py_check_socket_func(socket)
py_check_func(socketpair HEADERS sys/socket.h sys/types.h)

set(CMAKE_EXTRA_INCLUDE_FILES sys/types.h sys/socket.h netdb.h)
check_function_exists(getaddrinfo HAVE_GETADDRINFO)
check_function_exists(getnameinfo HAVE_GETNAMEINFO)
if(HAVE_GETADDRINFO)
    py_check_c_source_fails(check-getaddrinfo.c HAVE_BUGGY_GETADDRINFO)
endif()
if(NOT HAVE_GETADDRINFO OR HAVE_BUGGY_GETADDRINFO)
    message(FATAL_ERROR "You must get working getaddrinfo() function or pass -DENABLE_IPV6=FALSE option.")
endif()

#
# Check stat members.
#
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
if(HAVE_SYS_STAT_H)
    include(cmake/PyCheckStatMember.cmake)
    py_check_stat_member(st_birthtime)
    py_check_stat_member(st_blksize)
    py_check_stat_member(st_blocks)
    py_check_stat_member(st_flags)
    py_check_stat_member(st_gen)
    py_check_stat_member(st_rdev)
endif()

#
# Check passwd members.
#
check_include_file(pwd.h HAVE_PWD_H)
if(HAVE_PWD_H AND HAVE_SYS_TYPES_H)
    include(cmake/PyCheckPasswdMember.cmake)
    py_check_passwd_member(pw_gecos)
    py_check_passwd_member(pw_passwd)
endif()

set(_LARGEFILE_SOURCE 1)
set(_FILE_OFFSET_BITS 64)

#
# Check compiler flags.
#
include(CheckCCompilerFlag)
set(CMAKE_C_STANDARD 11)
set(COMPILER_FLAGS
    -Werror
    -fno-strict-overflow
    -fvisibility=hidden
    -fno-strict-aliasing
    -Wno-unused-result
    -Wno-unused-parameter
    -Wno-int-conversion
    -Wno-missing-field-initializers
    -Wsign-compare
    -Wstrict-prototypes
    -Werror=implicit-function-declaration
)
foreach(FLAG IN LISTS COMPILER_FLAGS)
    check_c_compiler_flag(${FLAG} _DEF)
    if(${_DEF})
        add_compile_options(${FLAG})
    endif()
    unset(_DEF)
endforeach()

if(CMAKE_C_COMPILER_ID MATCHES GNU)
    set(_GNU_SOURCE 1)
endif()

#
# Check for ANSI C headers.
#
check_include_files("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)

#
# Find thread library.
#
set(THREADS_PREFER_PTHREAD_FLAG 1)
find_package(Threads)
if(CMAKE_USE_PTHREADS_INIT)
    check_include_file(pthread.h HAVE_PTHREAD_H)

    # Defining _REENTRANT on system with POSIX threads should not hurt.
    set(_REENTRANT 1)

    include(cmake/PyCheckPthreadSymbol.cmake)
    py_check_pthread_symbol(pthread_init)
    py_check_pthread_symbol(pthread_kill)
    py_check_pthread_symbol(pthread_sigmask)
    py_check_pthread_symbol(pthread_condattr_setclock)
    py_check_pthread_symbol(PTHREAD_SCOPE_SYSTEM)

    set(CMAKE_EXTRA_INCLUDE_FILES pthread.h)
    check_type_size(pthread_key_t SIZEOF_PTHREAD_KEY_T)
    check_type_size(pthread_t SIZEOF_PTHREAD_T)
elseif(CMAKE_USE_WIN32_THREADS_INIT)
    set(NT_THREADS 1)
endif()

#
# Define global include paths before including sub-dirs.
#
include_directories(Include)
include_directories(Include/cpython)
include_directories(Include/internal)
include_directories(.)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

#
# Add subdirectories
#
add_subdirectory(Tools)
add_subdirectory(Objects)
add_subdirectory(Parser)
add_subdirectory(Python)

add_library(libpython_omit_frozen INTERFACE)
target_link_libraries(libpython_omit_frozen
  INTERFACE
    _modules
    _objects
    _python
    getbuildinfo
    parser
)

set(SOVERSION "1.0")
add_library(libpython STATIC)
target_link_libraries(libpython
  PRIVATE
    Threads::Threads

    frozen
    getpath
    libpython_omit_frozen
)

add_subdirectory(Modules)
add_subdirectory(Lib)
add_subdirectory(Programs)

#
# Generate pyconfig.h
#
configure_file(pyconfig.h.in pyconfig.h)
