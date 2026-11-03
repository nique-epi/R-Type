add_library(rtype_client_audio_music STATIC
  MusicPlayer.cpp
  Soundtrack.cpp
)

target_include_directories(rtype_client_audio_music PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_audio_music
  PUBLIC
    SFML::Audio
    rtype_client_audio_contract_music
    rtype_client_settings_module
    rtype_client_settings_values
    rtype_engine_time
    rtype_logging
  PRIVATE
    SFML::System
    rtype_client_asset_library
    rtype_client_audio_sounds
)

rtype_enable_warnings(rtype_client_audio_music)
