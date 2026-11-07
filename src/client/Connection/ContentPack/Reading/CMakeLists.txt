add_library(rtype_client_content_pack_reading STATIC PackContentReading.cpp)

target_include_directories(rtype_client_content_pack_reading PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_content_pack_reading
  PUBLIC
    rtype_client_entity_appearances
    rtype_game_entity_types
    rtype_identifiers
  PRIVATE
    rtype_client_exceptions
    rtype_logging
)

rtype_enable_warnings(rtype_client_content_pack_reading)
