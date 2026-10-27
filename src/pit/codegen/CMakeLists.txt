find_package(LLVM REQUIRED CONFIG)
message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION} from ${LLVM_DIR}")

llvm_map_components_to_libnames(pit_llvm_libs
    core
    support
    target
    mc
    native
    nativecodegen)

add_library(pit_codegen codegen.cpp)
add_library(pit::codegen ALIAS pit_codegen)

target_sources(pit_codegen PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/src
    FILES codegen.hpp)

target_link_libraries(pit_codegen
    PUBLIC  pit::ast pit::lexer
    PRIVATE cherry::options ${pit_llvm_libs})

target_include_directories(pit_codegen SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(pit_codegen PRIVATE ${LLVM_DEFINITIONS})

if(NOT LLVM_ENABLE_RTTI)
  target_compile_options(pit_codegen PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-fno-rtti>)
endif()

add_subdirectory(debug)
