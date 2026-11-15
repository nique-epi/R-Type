add_library(rtype_client_connection_casino STATIC CasinoChannel.cpp)

target_include_directories(rtype_client_connection_casino PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_casino
  PUBLIC
    rtype_client_session
    rtype_client_waiting_room
    rtype_engine_time
    rtype_network_casino_place
    rtype_network_lobby_state
    rtype_network_message_dispatcher
    rtype_network_roulette_bets
    rtype_network_roulette_round
  PRIVATE
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_connection_casino)
