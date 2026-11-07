add_library(rtype_game_entity_sheet_format INTERFACE)

target_include_directories(rtype_game_entity_sheet_format INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_entity_sheet_format
  INTERFACE
    rtype_engine_exceptions
    rtype_engine_time
    rtype_game_components
)

add_subdirectory(Texts)
add_subdirectory(Sheets)
add_subdirectory(Content)

add_library(rtype_game_entity_types INTERFACE)

target_link_libraries(rtype_game_entity_types
  INTERFACE
    rtype_game_entity_sheet_texts
    rtype_game_entity_sheets
    rtype_game_entity_content
    rtype_game_exceptions
)
