add_library(rtype_client_screens_module STATIC ClientScreensModule.cpp ClientScreens.cpp)

target_include_directories(rtype_client_screens_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_screens_module
  PUBLIC
    rtype_client_assets_module
    rtype_client_audio_contract
    rtype_client_connection
    rtype_client_entity_message_queue
    rtype_client_interface
    rtype_client_menus_module
    rtype_client_pilots
    rtype_client_screen_interfaces
    rtype_client_screen_registry
    rtype_client_application
    rtype_client_scrolling_background
    rtype_client_server_session
    rtype_client_session_loss_watch
    rtype_client_settings_module
    rtype_client_waiting_room
    rtype_client_window_module
    rtype_engine_commands
    rtype_engine_events
    rtype_engine_modules
    rtype_logging
    rtype_network_transport
  PRIVATE
    rtype_client_constants
    rtype_network_constants
    rtype_network_exceptions
    rtype_network_protocol
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_client_screens_module)
