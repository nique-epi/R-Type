add_subdirectory(Launch)
add_subdirectory(Play)
add_subdirectory(Squad)

add_executable(load_test_bots main.cpp)

target_link_libraries(load_test_bots
  PRIVATE
    rtype_server_platform
    rtype_tools_load_test_bots_squad
)

set_target_properties(load_test_bots PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

rtype_enable_warnings(load_test_bots)
