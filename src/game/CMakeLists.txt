add_subdirectory(Components)
add_subdirectory(Events)
add_subdirectory(Systems)

add_library(rtype_game STATIC World.cpp)

target_include_directories(rtype_game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game
  PUBLIC
    rtype_engine
    rtype_game_components
    rtype_game_events
    rtype_game_movement_system
    rtype_game_collision_system
)

rtype_enable_warnings(rtype_game)
