add_library(rtype_client_session_departure STATIC SessionDeparture.cpp)

target_include_directories(rtype_client_session_departure PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_session_departure
  PUBLIC
    rtype_client_session_outbox
    rtype_engine_time
    rtype_logging
    rtype_network_disconnection
  PRIVATE
    rtype_client_connection
)

rtype_enable_warnings(rtype_client_session_departure)
