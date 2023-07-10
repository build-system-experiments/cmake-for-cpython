# OBJECT libraries were introduced in 3.12
cmake_minimum_required(VERSION 3.12)

set_directory_properties(PROPERTIES COMPILE_DEFINITIONS -DPy_BUILD_CORE)

add_custom_target(regen-cases
    WORKING_DIRECTORY
        ${PROJECT_SOURCE_DIR}/Tools/cases_generator
    COMMAND
        "${PYTHON_FOR_REGEN}" generate_cases.py -l
    BYPRODUCT
        generated_cases.c.h
        opcode_metadata.h
)

add_custom_target(regen-opcode-targets
    COMMAND
        "${PYTHON_FOR_REGEN}"
            makeopcodetargets.py
            opcode_targets.h.new
    COMMAND
        "${UPDATE_FILE}"
            opcode_targets.h
            opcode_targets.h.new
    SOURCES
        ceval.c
)


#
# Frozen stuff.
#
configure_file("${PROJECT_SOURCE_DIR}/Python/frozen.c" frozen.c COPYONLY)
add_custom_target(frozen_modules
    WORKING_DIRECTORY
        "${PROJECT_SOURCE_DIR}/Tools/build"
    COMMENT
        "Generate header files for frozen modules."
    COMMAND
        "${PYTHON_FOR_REGEN}" freeze_modules.py
            --frozen-modules
            --root-dir="${PROJECT_BINARY_DIR}"
            --frozen-c="${CMAKE_CURRENT_BINARY_DIR}/frozen.c"
    BYPRODUCTS
        frozen_modules
        deepfreeze
    SOURCES
		"${PROJECT_SOURCE_DIR}/Lib/importlib/_bootstrap.py"
		"${PROJECT_SOURCE_DIR}/Lib/importlib/_bootstrap_external.py"
		"${PROJECT_SOURCE_DIR}/Lib/zipimport.py"
		"${PROJECT_SOURCE_DIR}/Lib/abc.py"
		"${PROJECT_SOURCE_DIR}/Lib/codecs.py"
		"${PROJECT_SOURCE_DIR}/Lib/io.py"
		"${PROJECT_SOURCE_DIR}/Lib/_collections_abc.py"
		"${PROJECT_SOURCE_DIR}/Lib/_sitebuiltins.py"
		"${PROJECT_SOURCE_DIR}/Lib/genericpath.py"
		"${PROJECT_SOURCE_DIR}/Lib/ntpath.py"
		"${PROJECT_SOURCE_DIR}/Lib/posixpath.py"
		"${PROJECT_SOURCE_DIR}/Lib/os.py"
		"${PROJECT_SOURCE_DIR}/Lib/site.py"
		"${PROJECT_SOURCE_DIR}/Lib/stat.py"
		"${PROJECT_SOURCE_DIR}/Lib/importlib/util.py"
		"${PROJECT_SOURCE_DIR}/Lib/importlib/machinery.py"
		"${PROJECT_SOURCE_DIR}/Lib/runpy.py"
		"${PROJECT_SOURCE_DIR}/Lib/__hello__.py"
		"${PROJECT_SOURCE_DIR}/Lib/__phello__/__init__.py"
		"${PROJECT_SOURCE_DIR}/Lib/__phello__/ham/__init__.py"
		"${PROJECT_SOURCE_DIR}/Lib/__phello__/ham/eggs.py"
		"${PROJECT_SOURCE_DIR}/Lib/__phello__/spam.py"
		"${PROJECT_SOURCE_DIR}/Tools/freeze/flag.py"
		"${PROJECT_SOURCE_DIR}/Python/frozen.c"
)
add_library(frozen OBJECT "${CMAKE_CURRENT_BINARY_DIR}/frozen.c" frozen_modules)
target_include_directories(frozen PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")


#
# Add _python library.
#
add_library(_python OBJECT
    Python-ast.c
    Python-tokenize.c
    _warnings.c
    asdl.c
    assemble.c
    ast.c
    ast_opt.c
    ast_unparse.c
    bltinmodule.c
    bootstrap_hash.c
    ceval.c
    ceval_gil.c
    codecs.c
    compile.c
    context.c
    dtoa.c
    dup2.c
    dynamic_annotations.c
    errors.c
    fileutils.c
    flowgraph.c
    formatter_unicode.c
    frame.c
    frozenmain.c
    future.c
    getargs.c
    getcompiler.c
    getcopyright.c
    getopt.c
    getplatform.c
    getversion.c
    hamt.c
    hashtable.c
    import.c
    importdl.c
    initconfig.c
    instrumentation.c
    intrinsics.c
    legacy_tracing.c
    marshal.c
    modsupport.c
    mysnprintf.c
    mystrtoul.c
    optimizer.c
    pathconfig.c
    perf_trampoline.c
    preconfig.c
    pyarena.c
    pyctype.c
    pyfpe.c
    pyhash.c
    pylifecycle.c
    pymath.c
    pystate.c
    pystrcmp.c
    pystrhex.c
    pystrtod.c
    pythonrun.c
    pytime.c
    specialize.c
    structmember.c
    suggestions.c
    symtable.c
    sysmodule.c
    thread.c
    traceback.c
    tracemalloc.c
)
target_include_directories(_python PRIVATE clinic)

#
# Dynamic loading.
#
set(CMAKE_REQUIRED_LIBRARIES dl)
check_function_exists(dlopen HAVE_DLOPEN)

set(CMAKE_REQUIRED_LIBRARIES dld)
check_function_exists(shl_load HAVE_SHL_LOAD)
unset(CMAKE_REQUIRED_LIBRARIES)

set(HAVE_DYNAMIC_LOADING 1)
if(WIN32)
    set(DYNLOADFILE dynload_win.c)
elseif(UNIX AND ${CMAKE_C_COMPILER_ID} MATCHES HP)
    set(DYNLOADFILE dynload_hpux.c)
elseif(HAVE_DLOPEN)
    set(DYNLOADFILE dynload_shlib.c)
    target_link_libraries(libs INTERFACE dl)
else()
    set(DYNLOADFILE dynload_stub.c)
    set(HAVE_DYNAMIC_LOADING 0)
endif()
target_sources(_python PRIVATE ${DYNLOADFILE})

if(APPLE)
    include("${PROJECT_SOURCE_DIR}/cmake/PyCheckFunc.cmake")
    py_check_func(_dyld_shared_cache_contains_path
        HEADERS mach-o/dyld.h
        RESULT_VAR HAVE_DYLD_SHARED_CACHE_CONTAINS_PATH
    )
    set(WITH_DYLD 1)
endif()

if(EMSCRIPTEN)
    target_sources(_python PRIVATE emscripten_signal.c)
endif()

if(PERF_SUPPORT)  # FIXME
    target_sources(_python PRIVATE asm_trampoline.S)
endif()
