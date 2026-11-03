add_library(rtype_network_statistics_round_trip STATIC
  RoundTripEstimator.cpp
  RoundTripMeter.cpp
)

target_include_directories(rtype_network_statistics_round_trip PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_statistics_round_trip
  PUBLIC
    rtype_network_heartbeat
    rtype_network_protocol
)

rtype_enable_warnings(rtype_network_statistics_round_trip)
