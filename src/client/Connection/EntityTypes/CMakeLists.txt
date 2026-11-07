add_library(rtype_client_connection_entity_types STATIC LocalContentReading.cpp SessionEntityTypes.cpp)

target_include_directories(rtype_client_connection_entity_types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_entity_types
  PUBLIC
    rtype_client_content_pack
    rtype_client_entity_appearances
    rtype_game_entity_types
    rtype_network_content
  PRIVATE
    rtype_identifiers
)

rtype_enable_warnings(rtype_client_connection_entity_types)
