add_library(rtype_client_sound_player STATIC SoundPlayer.cpp)

target_include_directories(rtype_client_sound_player PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sound_player
  PUBLIC
    rtype_client_audio_interfaces
    rtype_client_settings_service
    SFML::Audio
  PRIVATE
    rtype_client_asset_library
    rtype_client_audio_volume
    rtype_client_voice_choice
)

rtype_enable_warnings(rtype_client_sound_player)
