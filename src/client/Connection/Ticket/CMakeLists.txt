add_library(rtype_client_connection_ticket STATIC MasterTicketLink.cpp)

target_include_directories(rtype_client_connection_ticket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_ticket
  PUBLIC
    rtype_concurrency_latest_value
    rtype_network_connection_request
  PRIVATE
    rtype_client_connection_master_calls
    rtype_network_exceptions
    rtype_network_master_client
    rtype_network_master_messages
)

rtype_enable_warnings(rtype_client_connection_ticket)
