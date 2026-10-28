add_library(rtype_server_input_keeper STATIC InputKeeper.cpp)

target_include_directories(rtype_server_input_keeper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_input_keeper
  PUBLIC
    rtype_logging
    rtype_network_player_input
    rtype_server_player_roster
)

rtype_enable_warnings(rtype_server_input_keeper)
