add_library(rtype_client_book_motion STATIC BookMotion.cpp)

target_include_directories(rtype_client_book_motion PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_book_motion
  PUBLIC
    rtype_client_flight_log_interfaces
  PRIVATE
    rtype_client_easing
)

rtype_enable_warnings(rtype_client_book_motion)
