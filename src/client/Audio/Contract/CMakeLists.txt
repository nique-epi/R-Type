add_library(rtype_client_audio_contract STATIC
  NullMusicPlayer.cpp
  NullSoundPlayer.cpp
  SilentAudioService.cpp
  SyncedClock.cpp
)

target_include_directories(rtype_client_audio_contract PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_audio_contract PUBLIC rtype_engine_time)

rtype_enable_warnings(rtype_client_audio_contract)
