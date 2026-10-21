add_library(rtype_game_ship_control_constants INTERFACE)

target_include_directories(rtype_game_ship_control_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(rtype_game_ship_control STATIC ShipControl.cpp)

target_include_directories(rtype_game_ship_control PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_ship_control
  PUBLIC
    rtype_engine_time
    rtype_game_components
    rtype_game_ship_control_constants
  PRIVATE
    rtype_game
)

rtype_enable_warnings(rtype_game_ship_control)
