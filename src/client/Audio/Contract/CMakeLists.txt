add_subdirectory(Sounds)
add_subdirectory(Music)
add_subdirectory(Service)

add_library(rtype_client_audio_contract INTERFACE)

target_link_libraries(rtype_client_audio_contract
  INTERFACE
    rtype_client_audio_contract_music
    rtype_client_audio_contract_service
    rtype_client_audio_contract_sounds
)
