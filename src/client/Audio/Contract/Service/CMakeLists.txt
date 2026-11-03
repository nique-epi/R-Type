add_library(rtype_client_audio_contract_service STATIC
  SilentAudioService.cpp
)

target_include_directories(rtype_client_audio_contract_service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_audio_contract_service
  PUBLIC
    rtype_client_audio_contract_music
    rtype_client_audio_contract_sounds
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_audio_contract_service)
