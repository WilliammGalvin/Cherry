add_library(cherry_options INTERFACE)
add_library(cherry::options ALIAS cherry_options)

target_compile_options(cherry_options INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Wconversion>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>)
