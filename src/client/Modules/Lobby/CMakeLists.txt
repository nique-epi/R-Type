add_library(rtype_client_lobby_module STATIC ClientLobbyModule.cpp)

target_include_directories(rtype_client_lobby_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_lobby_module
  PUBLIC
    rtype_client_screen_registry
    rtype_engine_modules
  PRIVATE
    rtype_client_input
    rtype_client_lobby_screen
    rtype_client_constants
    rtype_client_player_preferences
)

rtype_enable_warnings(rtype_client_lobby_module)
