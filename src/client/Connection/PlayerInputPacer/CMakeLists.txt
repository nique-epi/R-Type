add_library(rtype_client_player_input_pacer STATIC PlayerInputPacer.cpp)

target_include_directories(rtype_client_player_input_pacer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_player_input_pacer
  PUBLIC
    rtype_client_connection
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_player_input_pacer)
