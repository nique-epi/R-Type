add_library(rtype_game_entity_types STATIC EntitySheetReader.cpp EntitySheets.cpp SheetTextReading.cpp)

target_include_directories(rtype_game_entity_types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_entity_types
  PUBLIC
    rtype_engine_data_files
    rtype_engine_time
    rtype_game_behaviours
    rtype_game_components
    rtype_game_exceptions
  PRIVATE
    rtype_identifiers
    rtype_logging
    rtype_game_ship_control_constants
)

rtype_enable_warnings(rtype_game_entity_types)
