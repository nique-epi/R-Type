add_library(rtype_game_level_ground STATIC GroundLookup.cpp GroundReading.cpp)

target_include_directories(rtype_game_level_ground PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_level_ground
  PUBLIC
    rtype_game_components
    rtype_game_level_ldtk
  PRIVATE
    rtype_engine_data_files
)

rtype_enable_warnings(rtype_game_level_ground)
