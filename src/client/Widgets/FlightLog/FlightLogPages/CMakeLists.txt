add_library(rtype_client_flight_log_pages STATIC FlightLogPages.cpp)

target_include_directories(rtype_client_flight_log_pages PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_flight_log_pages
  PUBLIC
    rtype_client_flight_log_interfaces
  PRIVATE
    rtype_client_text_measures
)

rtype_enable_warnings(rtype_client_flight_log_pages)
