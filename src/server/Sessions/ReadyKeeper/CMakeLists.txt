add_library(rtype_server_ready_keeper STATIC ReadyKeeper.cpp)

target_include_directories(rtype_server_ready_keeper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_ready_keeper
  PUBLIC
    rtype_network_ready_state
    rtype_server_player_roster
)

rtype_enable_warnings(rtype_server_ready_keeper)
