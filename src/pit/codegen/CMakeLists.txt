find_package(LLVM REQUIRED CONFIG)
message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION} from ${LLVM_DIR}")

llvm_map_components_to_libnames(pit_llvm_libs
    core
    support
    target
    mc
    native
    nativecodegen
)

cherry_add_component(pit_codegen
    ALIAS   pit::codegen
    SOURCES codegen.cpp
    HEADERS codegen.hpp
    DEPENDS pit::ast pit::lexer
)

target_include_directories(pit_codegen SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(pit_codegen PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(pit_codegen PRIVATE ${pit_llvm_libs})

if(NOT LLVM_ENABLE_RTTI)
  target_compile_options(pit_codegen PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-fno-rtti>)
endif()
