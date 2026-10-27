add_library(rtype_client_end_screen STATIC EndScreen.cpp)

target_include_directories(rtype_client_end_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_end_screen
  PUBLIC
    rtype_client_connection
    rtype_client_flight_log_interfaces
    rtype_client_interface
    rtype_client_screen_interfaces
    rtype_client_window
    SFML::Graphics
  PRIVATE
    rtype_client_end_choices
    rtype_client_flight_log_painting
    rtype_client_flight_log_timeline
    rtype_client_interface_pixel
    rtype_client_taunts
)

rtype_enable_warnings(rtype_client_end_screen)
