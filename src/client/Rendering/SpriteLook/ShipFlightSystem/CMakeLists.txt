add_library(rtype_client_ship_flight_system STATIC ShipFlightSystem.cpp)

target_include_directories(rtype_client_ship_flight_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_ship_flight_system
  PUBLIC
    rtype_engine
    rtype_client_sprite_look
  PRIVATE
    rtype_client_easing
    rtype_game_ship_control_constants
)

rtype_enable_warnings(rtype_client_ship_flight_system)
