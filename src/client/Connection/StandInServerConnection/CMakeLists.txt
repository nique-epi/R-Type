add_library(rtype_client_stand_in_server_connection STATIC StandInServerConnection.cpp)

target_include_directories(rtype_client_stand_in_server_connection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_stand_in_server_connection
  PUBLIC
    rtype_client_connection
  PRIVATE
    rtype_logging
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_stand_in_server_connection)
