add_library(rtype_client_screen_registry STATIC
  ClientScreenRegistryModule.cpp
  ScreenBuilding.cpp
)

target_include_directories(rtype_client_screen_registry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screen_registry
  PUBLIC
    rtype_client_asset_library
    rtype_client_connection
    rtype_client_exceptions
    rtype_client_game_window
    rtype_client_interface
    rtype_client_music_player
    rtype_client_replication_interfaces
    rtype_client_screen_interfaces
    rtype_client_scrolling_background
    rtype_client_server_connection
    rtype_client_settings
    rtype_client_sound_player
    rtype_client_soundtrack
    rtype_client_window
    rtype_engine_modules
    rtype_network_transport
  PRIVATE
    rtype_client_interface_canvas
    rtype_client_loading_screen
    rtype_client_module_constants
    rtype_client_platform
)

rtype_enable_warnings(rtype_client_screen_registry)
