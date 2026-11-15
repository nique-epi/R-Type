add_library(rtype_client_casino_roulette_wheel STATIC WheelMotion.cpp)

target_include_directories(rtype_client_casino_roulette_wheel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_casino_roulette_wheel
  PUBLIC
    SFML::System
    rtype_client_interface
    rtype_engine_time
    rtype_network_protocol
    rtype_network_roulette_round
  PRIVATE
    rtype_client_widgets
)

rtype_enable_warnings(rtype_client_casino_roulette_wheel)
