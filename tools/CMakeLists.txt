add_executable(orchard_cli main.cpp)
set_target_properties(orchard_cli PROPERTIES OUTPUT_NAME orchard)

target_link_libraries(orchard_cli PRIVATE
  cherry::options
  orchard::orchard
)

install(TARGETS orchard_cli RUNTIME DESTINATION bin)
