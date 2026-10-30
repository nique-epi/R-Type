add_library(rtype_client_audio_service STATIC AudioService.cpp)

target_include_directories(rtype_client_audio_service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_audio_service
  PUBLIC
    rtype_client_asset_library
    rtype_client_audio_interfaces
    rtype_client_music_player
    rtype_client_settings
    rtype_client_sound_player
    rtype_client_soundtrack
)

rtype_enable_warnings(rtype_client_audio_service)
