rtype_add_test(NAME client_input_tests LIBS rtype_client_input)
target_include_directories(client_input_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR})

add_subdirectory(Commands)
add_subdirectory(Keys)
