add_library(rtype_server_discard_report STATIC DiscardReport.cpp)

target_include_directories(rtype_server_discard_report PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_discard_report
  PUBLIC
    rtype_concurrency_bounded_queue
    rtype_logging
    rtype_network_transport
    rtype_server_datagram_sorter
  PRIVATE
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_discard_report)
