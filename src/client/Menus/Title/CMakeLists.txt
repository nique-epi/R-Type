add_library(rtype_client_title STATIC
  ConnectionScreen.cpp
  EndpointForm.cpp
  DialogMenus.cpp
  IntroScreen.cpp
  TitleScreen.cpp
)

target_include_directories(rtype_client_title PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_title
  PUBLIC
    SFML::Graphics
    rtype_client_audio_contract_music
    rtype_client_audio_contract_sounds
    rtype_client_camera_shake
    rtype_client_handshake
    rtype_client_interface
    rtype_client_intro_interfaces
    rtype_client_menu
    rtype_client_screens_module
    rtype_client_squadron
    rtype_client_update_contract
    rtype_client_window_display
    rtype_logging
    rtype_network_constants
    rtype_network_protocol
  PRIVATE
    rtype_client_intro_cast_builder
    rtype_client_intro_cues
    rtype_client_intro_overlay
    rtype_client_intro_painting
    rtype_client_intro_timeline
    rtype_client_menu_builder
    rtype_client_text_field
    rtype_identifiers
    rtype_network_transport
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_client_title)
