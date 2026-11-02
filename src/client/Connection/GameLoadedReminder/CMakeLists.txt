add_library(rtype_client_game_loaded_reminder STATIC GameLoadedReminder.cpp)

target_include_directories(rtype_client_game_loaded_reminder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_game_loaded_reminder
  PUBLIC
    rtype_client_connection
    rtype_engine_time
    rtype_network_game_loaded
)

rtype_enable_warnings(rtype_client_game_loaded_reminder)
