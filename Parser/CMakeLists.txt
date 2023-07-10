set_directory_properties(PROPERTIES COMPILE_DEFINITIONS -DPy_BUILD_CORE)

add_library(pegen
  OBJECT
    action_helpers.c
    parser.c
    peg_api.c
    pegen.c
    pegen_errors.c
    string_parser.c
)
add_library(parser
  OBJECT
    myreadline.c
    token.c
    tokenizer.c
    $<TARGET_OBJECTS:pegen>
)
