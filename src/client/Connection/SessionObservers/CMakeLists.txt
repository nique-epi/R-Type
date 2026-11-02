add_library(rtype_client_session_observers STATIC SessionObservers.cpp)

target_include_directories(rtype_client_session_observers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_session_observers
  PUBLIC
    rtype_client_connection
    rtype_engine_time
    rtype_network_lobby_state
)

rtype_enable_warnings(rtype_client_session_observers)
