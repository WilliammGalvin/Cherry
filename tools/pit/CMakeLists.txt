add_executable(pit main.cpp)

target_link_libraries(pit
    PRIVATE
        pit::compiler
        cherry::common
        cherry::warnings
)
