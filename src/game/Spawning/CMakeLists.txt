add_subdirectory(BydoKind)

add_library(rtype_game_spawning_constants INTERFACE)

target_include_directories(rtype_game_spawning_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(rtype_game_spawning STATIC ShipStartPosition.cpp BydoSpawnPosition.cpp)

target_include_directories(rtype_game_spawning PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_spawning
  PUBLIC
    rtype_game
    rtype_game_components
    rtype_game_spawning_constants
  PRIVATE
    rtype_game_exceptions
)

rtype_enable_warnings(rtype_game_spawning)
