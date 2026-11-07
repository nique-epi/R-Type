add_library(rtype_game_entity_content STATIC ContentFiles.cpp NumberedEntityTypes.cpp)

target_include_directories(rtype_game_entity_content PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_entity_content
  PUBLIC
    rtype_game_components
    rtype_game_entity_sheet_texts
    rtype_identifiers
  PRIVATE
    rtype_engine_data_files
    rtype_game_entity_sheet_format
    rtype_logging
)

rtype_enable_warnings(rtype_game_entity_content)
