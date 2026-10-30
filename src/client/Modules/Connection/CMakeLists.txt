add_library(rtype_client_connection_module STATIC ClientConnectionModule.cpp)

target_include_directories(rtype_client_connection_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_module
  PUBLIC
    rtype_client_datagram_router
    rtype_client_entity_message_queue
    rtype_client_network_thread
    rtype_client_server_session
    rtype_client_waiting_room
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_connection
    rtype_client_constants
    rtype_client_module_constants
    rtype_network_constants
    rtype_network_transport
)

rtype_enable_warnings(rtype_client_connection_module)
