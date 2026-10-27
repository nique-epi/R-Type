add_library(rtype_client_flight_log_timeline STATIC FlightLogTimeline.cpp)

target_include_directories(rtype_client_flight_log_timeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_flight_log_timeline
  PUBLIC
    rtype_client_flight_log_interfaces
  PRIVATE
    rtype_client_audio_interfaces
    rtype_client_typing
)

rtype_enable_warnings(rtype_client_flight_log_timeline)
