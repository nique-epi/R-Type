add_library(rtype_client_audio_sounds STATIC
  AudioVolume.cpp
  SoundPlayer.cpp
  VoiceChoice.cpp
)

target_include_directories(rtype_client_audio_sounds PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_audio_sounds
  PUBLIC
    SFML::Audio
    rtype_client_audio_contract_music
    rtype_client_audio_contract_sounds
    rtype_client_settings_module
    rtype_client_settings_values
  PRIVATE
    rtype_client_asset_library
)

rtype_enable_warnings(rtype_client_audio_sounds)
