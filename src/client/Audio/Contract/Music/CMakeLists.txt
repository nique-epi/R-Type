add_library(rtype_client_audio_contract_music STATIC
  NullMusicPlayer.cpp
  SyncedClock.cpp
)

target_include_directories(rtype_client_audio_contract_music PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_audio_contract_music
  PUBLIC
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_audio_contract_music)
