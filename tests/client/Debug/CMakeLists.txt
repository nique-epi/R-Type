if(NOT RTYPE_MODULE_DEBUG)
  return()
endif()

rtype_add_test(NAME client_debug_tests SOURCES ${RTYPE_TEST_SOURCE_DIR}/client/Screens/KeyPress.cpp LIBS rtype_client_debug_module)
target_include_directories(client_debug_tests PRIVATE ${RTYPE_TEST_SOURCE_DIR}/client/Screens)

add_subdirectory(Panel)
add_subdirectory(Journal)
