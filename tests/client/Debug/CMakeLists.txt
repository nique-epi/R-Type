if(NOT RTYPE_MODULE_DEBUG)
  return()
endif()

rtype_add_test(NAME client_debug_tests SOURCES LagometerTest.cpp LagometerFixtures.cpp LagometerToggleTest.cpp LagometerTextTest.cpp LagometerPaintingTest.cpp ${RTYPE_TEST_SOURCE_DIR}/client/Screens/KeyPress.cpp LIBS rtype_client_debug_module)
target_include_directories(client_debug_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/client/Screens)
