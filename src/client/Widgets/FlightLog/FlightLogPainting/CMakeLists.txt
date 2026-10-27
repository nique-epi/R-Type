add_library(rtype_client_flight_log_painting STATIC FlightLogPainting.cpp)

target_include_directories(rtype_client_flight_log_painting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_flight_log_painting
  PUBLIC
    rtype_client_flight_log_interfaces
  PRIVATE
    rtype_client_book_motion
    rtype_client_book_painting
    rtype_client_easing
    rtype_client_end_choices
    rtype_client_mission_log_page
    rtype_client_ranking_page
    rtype_client_taunt_label
)

rtype_enable_warnings(rtype_client_flight_log_painting)
