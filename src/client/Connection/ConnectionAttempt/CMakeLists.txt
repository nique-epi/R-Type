add_library(rtype_client_connection_attempt STATIC ConnectionAttempt.cpp)

target_include_directories(rtype_client_connection_attempt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_attempt
  PUBLIC
    rtype_client_connection
    rtype_engine_time
    rtype_network_connection_request
)

rtype_enable_warnings(rtype_client_connection_attempt)
