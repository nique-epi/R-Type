add_library(rtype_client_silence_warning STATIC SilenceWarning.cpp)

target_include_directories(rtype_client_silence_warning PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_silence_warning
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_entity_sprites
    rtype_client_replication_interfaces
)

rtype_enable_warnings(rtype_client_silence_warning)
