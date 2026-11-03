add_library(rtype_client_key_chip STATIC KeyChip.cpp)

target_include_directories(rtype_client_key_chip PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_key_chip
  PUBLIC
    rtype_client_audio_contract_sounds
    rtype_client_button
    rtype_client_widgets
    rtype_client_window_display
    SFML::Window
  PRIVATE
    rtype_client_interface_pixel
)

rtype_enable_warnings(rtype_client_key_chip)
