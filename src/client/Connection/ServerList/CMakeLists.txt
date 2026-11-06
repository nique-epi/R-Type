add_library(rtype_client_server_list STATIC
  GameCatalog.cpp
  MasterLink.cpp
  ServerCatalog.cpp
  ServerPingProbe.cpp
)

target_include_directories(rtype_client_server_list PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_server_list
  PUBLIC
    rtype_client_transport
    rtype_concurrency_latest_value
    rtype_engine_time
    rtype_network_master_client
    rtype_network_master_messages
    rtype_network_server_information
    rtype_network_transport
  PRIVATE
    rtype_network_datagram_codec
    rtype_network_exceptions
    rtype_network_protocol
    rtype_network_server_information_request
    rtype_network_statistics_round_trip
)

rtype_enable_warnings(rtype_client_server_list)
