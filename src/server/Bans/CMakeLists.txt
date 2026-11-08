add_library(rtype_server_bans STATIC BanList.cpp CurrentSecond.cpp)

target_include_directories(rtype_server_bans PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_bans
  PUBLIC
    rtype_concurrency_latest_value
  PRIVATE
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_server_bans)
