add_library(rtype_server_delayed_inputs STATIC DelayedInputs.cpp)

target_include_directories(rtype_server_delayed_inputs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_delayed_inputs
  PUBLIC
    rtype_network_player_input
    rtype_server_player_roster
    rtype_server_sessions
  PRIVATE
    rtype_server_constants
    rtype_server_exceptions
)

rtype_enable_warnings(rtype_server_delayed_inputs)
