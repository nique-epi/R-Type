add_library(rtype_client_lobby STATIC ClientLobbyModule.cpp)

target_include_directories(rtype_client_lobby PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_lobby
  PUBLIC
    rtype_client_screen_registry
    rtype_engine_modules
  PRIVATE
    rtype_client_localized_key_name
    rtype_client_lobby_screen
    rtype_client_module_constants
    rtype_client_player_preferences
)

rtype_enable_warnings(rtype_client_lobby)
