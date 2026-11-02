add_library(rtype_client_session_acknowledgements STATIC SessionAcknowledgements.cpp)

target_include_directories(rtype_client_session_acknowledgements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_session_acknowledgements
  PUBLIC
    rtype_engine_time
    rtype_network_protocol
    rtype_network_reliability
)

rtype_enable_warnings(rtype_client_session_acknowledgements)
