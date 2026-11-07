add_library(rtype_server_content_module STATIC
  ContentPackDesk.cpp
  ServerContentModule.cpp
)

target_include_directories(rtype_server_content_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_content_module
  PUBLIC
    rtype_engine_modules
    rtype_engine_system_clock
    rtype_engine_time
    rtype_logging
    rtype_network_content
    rtype_network_handshake_rate_limit
    rtype_network_http
    rtype_server_network_module
  PRIVATE
    rtype_engine_paths
    rtype_game_entity_types
    rtype_network_constants
    rtype_network_exceptions
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_content_module)
