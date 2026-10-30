add_library(rtype_client_game STATIC ClientGameModule.cpp)

target_include_directories(rtype_client_game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_game
  PUBLIC
    rtype_client_screen_registry
    rtype_engine_modules
  PRIVATE
    rtype_client_end_screen
    rtype_client_entity_layer
    rtype_client_game_screen
    rtype_client_module_constants
    rtype_client_player_preferences
    rtype_client_screen_assets
)

rtype_enable_warnings(rtype_client_game)
