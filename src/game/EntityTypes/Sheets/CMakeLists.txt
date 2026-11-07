add_library(rtype_game_entity_sheets STATIC EntitySheetReader.cpp EntitySheets.cpp SheetActionReading.cpp VolleySheetReading.cpp)

target_include_directories(rtype_game_entity_sheets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_entity_sheets
  PUBLIC
    rtype_engine_data_files
    rtype_game_behaviours
    rtype_game_components
    rtype_game_entity_sheet_format
    rtype_game_entity_sheet_texts
  PRIVATE
    rtype_engine_time
    rtype_game_exceptions
    rtype_game_ship_control_constants
    rtype_identifiers
)

rtype_enable_warnings(rtype_game_entity_sheets)
