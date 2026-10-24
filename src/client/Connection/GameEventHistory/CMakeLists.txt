add_library(rtype_client_game_event_history STATIC GameEventHistory.cpp)

target_include_directories(rtype_client_game_event_history PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_game_event_history
  PUBLIC
    rtype_client_connection
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_game_event_history)
