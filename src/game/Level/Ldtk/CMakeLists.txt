find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_game_level_ldtk STATIC LdtkPlacementReading.cpp)

target_include_directories(rtype_game_level_ldtk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_level_ldtk
  PUBLIC
    rtype_game_components
  PRIVATE
    nlohmann_json::nlohmann_json
    rtype_engine_data_files
    rtype_game_exceptions
    rtype_identifiers
)

rtype_enable_warnings(rtype_game_level_ldtk)
