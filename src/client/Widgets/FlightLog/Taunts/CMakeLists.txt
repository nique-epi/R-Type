add_library(rtype_client_taunts STATIC Taunts.cpp)

target_include_directories(rtype_client_taunts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_taunts
  PUBLIC
    rtype_client_flight_log_interfaces
  PRIVATE
    rtype_client_pilot_name
)

rtype_enable_warnings(rtype_client_taunts)
