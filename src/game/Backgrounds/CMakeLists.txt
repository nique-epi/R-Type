add_library(rtype_game_backgrounds STATIC BackgroundFiles.cpp)

target_include_directories(rtype_game_backgrounds PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_backgrounds
  PUBLIC
    rtype_identifiers
  PRIVATE
    rtype_engine_data_files
    rtype_logging
)

rtype_enable_warnings(rtype_game_backgrounds)
