if(NOT RTYPE_MODULE_THREAT)
  return()
endif()

target_sources(server_game_module_tests PRIVATE ThreatModuleTest.cpp)
target_include_directories(server_game_module_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(server_game_module_tests PRIVATE rtype_server_threat_module)
