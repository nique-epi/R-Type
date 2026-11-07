add_subdirectory(Contract)

if(NOT RTYPE_MODULE_AUDIO)
  return()
endif()

add_subdirectory(Sounds)
add_subdirectory(Music)

add_library(rtype_client_audio_module STATIC
  AudioService.cpp
  ClientAudioModule.cpp
)

target_include_directories(rtype_client_audio_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_audio_module
  PUBLIC
    rtype_client_asset_library
    rtype_client_audio_contract
    rtype_client_audio_music
    rtype_client_audio_sounds
    rtype_client_settings_module
    rtype_engine_install_context
    rtype_engine_modules
    rtype_engine_time
    rtype_logging
  PRIVATE
    rtype_client_constants
)

rtype_enable_warnings(rtype_client_audio_module)
