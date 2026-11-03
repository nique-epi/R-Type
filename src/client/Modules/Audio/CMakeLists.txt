add_library(rtype_client_audio_module STATIC ClientAudioModule.cpp)

target_include_directories(rtype_client_audio_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_audio_module
  PUBLIC
    rtype_client_assets_module
    rtype_client_audio_interfaces
    rtype_client_settings_module
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_audio_service
    rtype_client_constants
    rtype_client_silent_audio_service
)

rtype_enable_warnings(rtype_client_audio_module)
