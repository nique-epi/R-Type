add_subdirectory(Contract)

add_library(rtype_client_audio_module STATIC
  AudioService.cpp
  AudioVolume.cpp
  ClientAudioModule.cpp
  MusicPlayer.cpp
  SoundPlayer.cpp
  Soundtrack.cpp
  VoiceChoice.cpp
)

target_include_directories(rtype_client_audio_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_audio_module
  PUBLIC
    SFML::Audio
    rtype_client_assets_module
    rtype_client_audio_contract
    rtype_client_settings_module
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_constants
)

rtype_enable_warnings(rtype_client_audio_module)
