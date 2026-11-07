add_library(rtype_game_entity_sheet_texts STATIC SheetTextReading.cpp)

target_include_directories(rtype_game_entity_sheet_texts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_entity_sheet_texts
  PUBLIC
    rtype_engine_data_files
  PRIVATE
    rtype_game_entity_sheet_format
    rtype_game_exceptions
    rtype_identifiers
    rtype_logging
)

rtype_enable_warnings(rtype_game_entity_sheet_texts)
