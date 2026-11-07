if(NOT RTYPE_MODULE_METRICS)
  return()
endif()

target_sources(client_module_tests PRIVATE ClientMetricsModuleTest.cpp ClientMetricsModuleFixture.cpp ${RTYPE_TEST_SOURCE_DIR}/network/Statistics/TrafficFixtures.cpp)
target_include_directories(client_module_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/network/Statistics)
target_link_libraries(client_module_tests PRIVATE rtype_client_metrics_module)
