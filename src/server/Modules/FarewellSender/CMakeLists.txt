add_library(rtype_server_farewell_sender STATIC FarewellSender.cpp)

target_include_directories(rtype_server_farewell_sender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_farewell_sender
  PUBLIC
    rtype_engine_time
    rtype_server_application_interfaces
    rtype_server_games
  PRIVATE
    rtype_network_disconnection
    rtype_server_module_constants
    rtype_server_replication_constants
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_farewell_sender)
