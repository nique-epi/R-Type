add_library(rtype_server_tick_datagrams STATIC TickDatagrams.cpp)

target_include_directories(rtype_server_tick_datagrams PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_tick_datagrams
  PUBLIC
    rtype_network_transport
    rtype_server_application
)

rtype_enable_warnings(rtype_server_tick_datagrams)
