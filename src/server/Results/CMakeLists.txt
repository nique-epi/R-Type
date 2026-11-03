add_library(rtype_server_results_module STATIC
  GameResultKeeper.cpp
  LeaderboardDesk.cpp
  ResultsModule.cpp
)

target_include_directories(rtype_server_results_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_results_module
  PUBLIC
    rtype_engine_install_context
    rtype_server_instances
    rtype_server_storage_module
  PRIVATE
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_results_module)
