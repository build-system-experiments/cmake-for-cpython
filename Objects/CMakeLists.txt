# OBJECT libraries were introduced in 3.12
cmake_minimum_required(VERSION 3.12)

set_directory_properties(PROPERTIES COMPILE_DEFINITIONS -DPy_BUILD_CORE)

add_library(_objects OBJECT
    abstract.c
    boolobject.c
    bytearrayobject.c
    bytes_methods.c
    bytesobject.c
    call.c
    capsule.c
    cellobject.c
    classobject.c
    codeobject.c
    complexobject.c
    descrobject.c
    dictobject.c
    enumobject.c
    exceptions.c
    fileobject.c
    floatobject.c
    frameobject.c
    funcobject.c
    genericaliasobject.c
    genobject.c
    interpreteridobject.c
    iterobject.c
    listobject.c
    longobject.c
    memoryobject.c
    methodobject.c
    moduleobject.c
    namespaceobject.c
    object.c
    obmalloc.c
    odictobject.c
    picklebufobject.c
    rangeobject.c
    setobject.c
    sliceobject.c
    structseq.c
    tupleobject.c
    typeobject.c
    typevarobject.c
    unicodectype.c
    unicodeobject.c
    unionobject.c
    weakrefobject.c
)
target_include_directories(_objects PRIVATE clinic stringlib)
