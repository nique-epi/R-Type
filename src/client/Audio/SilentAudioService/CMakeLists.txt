add_library(rtype_client_silent_audio_service STATIC SilentAudioService.cpp)

target_include_directories(rtype_client_silent_audio_service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_silent_audio_service
  PUBLIC
    rtype_client_audio_interfaces
    rtype_client_null_music_player
    rtype_client_null_sound_player
)

rtype_enable_warnings(rtype_client_silent_audio_service)
