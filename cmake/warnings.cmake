add_library(cherry_warnings INTERFACE)
add_library(cherry::warnings ALIAS cherry_warnings)

target_compile_options(cherry_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -Wall -Wextra -Wpedantic
        -Wshadow -Wconversion -Wsign-conversion
        -Wnon-virtual-dtor -Wold-style-cast -Wcast-align
        -Wunused -Woverloaded-virtual -Wdouble-promotion
        -Werror=switch -Werror=return-type
        $<$<BOOL:${CHERRY_WERROR}>:-Werror>
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /permissive-
        $<$<BOOL:${CHERRY_WERROR}>:/WX>
    >
)

