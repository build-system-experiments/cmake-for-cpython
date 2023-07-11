add_library(_hacl_md5 OBJECT Hacl_Hash_MD5.c)
add_library(_hacl_sha1 OBJECT Hacl_Hash_SHA1.c)
add_library(_hacl_sha2 OBJECT Hacl_Hash_SHA2.c)
target_include_directories(_hacl PUBLIC include)
set_directory_properties(PROPERTIES
  COMPILE_OPTIONS "-D_BSD_SOURCE;-D_DEFAULT_SOURCE"
  INCLUDE_DIRECTORIES include
)
